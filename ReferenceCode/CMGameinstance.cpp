// Fill out your copyright notice in the Description page of Project Settings.


#include "CMGameInstance.h"
#include "TBManager.h"
#include "CMTimeManager.h"
#include "CMServerTimeManager.h"
#include "CMAccountManager.h"
#include "CMItemInfoManager.h"
#include "CMCharInfoManager.h"
#include "CMStageInfoManager.h"
#include "CMTacticInfoManager.h"
#include "CMSoundManager.h"
#include "CMGameActorManager.h"
#include "CMUISystemManager.h"
#include "CMGlobal.h"
#include "Engine.h"
#include "CMConsoleCommands.h"
#include "CMDataAssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "CMFacilityInfoManager.h"
#include "CMGameMode.h"
#include "CMGameSingleton.h"
#include "UISystemData.h"
#include "CMCommon.h"
#include "CMPageInfo.h"
#include "CMPanelDataBase.h"
#include "NetManager.h"
#include "CMTutorialManager.h"
#include "CMResourceUtility.h"


///////////////////////////////////////////////////////////////////////////////////////////////
// ChunkDownloader
#include "ChunkDownloader.h"
#include "Misc/CoreDelegates.h"
#include "AssetRegistryModule.h"
#include "Math/UnitConversion.h"


TMap<EUI_GameModeType, TFunction<bool(FName& OutPath, TMap<FString, FString>& OutLevelOptions, UCMGameInstance* InGameInstance)>> UCMGameInstance::OpenLevelPathPrevHandlers =
{
	{
		EUI_GameModeType::Lobby_Main, [](FName& OutPath, TMap<FString, FString>& OutLevelOptions, UCMGameInstance* InGameInstance)
		{
			CM_LOG(Log, "OpenLevelPrevTryFunctions");
			UCMAccountManager* AccountManager_Local = InGameInstance->GetAccountManager();
			if (IsValid(AccountManager_Local))
			{
				// 레벨 패스 수정
				DECLARE_TABLE();
				UCMCharInfoManager* CharInfoManager_ = InGameInstance->GetCharInfoManager();
			
				UTBCharacter_LobbyLocation* const TbCharLoc = TableManager->character_lobbylocation;
				if (IsValid(CharInfoManager_) && IsValid(TbCharLoc))
				{
					uint64 representativeCharID = AccountManager_Local->GetAccount()->RepresentativeCharacterID;
			
					UCMCharInfo* CharInfo = CharInfoManager_->GetCharInfo(representativeCharID);
					if (IsValid(CharInfo))
					{
						UTBCharacter_LobbyLocation_Record* LobbyLocRec = TbCharLoc->Find(CharInfo->GetInfo().CharID);
						if (IsValid(LobbyLocRec))
						{
							OutPath = FName(LobbyLocRec->PathLevel);
							if(OutLevelOptions.Contains(FCMLevelInfo::LevelOptionKey_GameMode))
							{
								OutLevelOptions[FCMLevelInfo::LevelOptionKey_GameMode] = TEXT("/Game/Game/Level/GM_Lobby_BP.GM_Lobby_BP_C");
							}
							else
							{
								OutLevelOptions.Emplace(FCMLevelInfo::LevelOptionKey_GameMode, TEXT("/Game/Game/Level/GM_Lobby_BP.GM_Lobby_BP_C"));
							}
							return true;
						}
					}
					//return true; // true: 수정한 패스 수정 함, false: 패스 수정 안함
				}
			}
			return false;
		}
	},
	{
		EUI_GameModeType::Tactics, [](FName& OutPath, TMap<FString, FString>& OutLevelOptions, UCMGameInstance* InGameInstance)
		{
			CM_LOG(Log, "OpenLevelPrevTryFunctions ModeType:Tactics");
			UCMStageInfoManager* StageInfoManager_ = InGameInstance->GetStageInfoManager();
			if( IsValid(StageInfoManager_))
			{
				// 스테이지 클리어 후 "다음 스테이지" 를 눌렀을 경우 기존 전술맵 스테이지를 부르는 경우가 있어서 추가함
				
				const uint32 SelectedStageID = StageInfoManager_->GetSelectedStageID(); 
				const FString StrStageID = FString::FromInt(SelectedStageID);
				if( false == OutPath.ToString().Contains(StrStageID))
				{
					OutPath = GET_CM_RESOURCEUTILITY()->GetStageTacticsLevelPath(SelectedStageID);
				}
				return true;
			}
			return false;
		}
	},
	{
		EUI_GameModeType::Daily_Dungeon, [](FName& OutPath, TMap<FString, FString>& OutLevelOptions, UCMGameInstance* InGameInstance)
		{
			UCMGameInstance* GameInstance = InGameInstance;
			UCMStageInfoManager* StageInfoManager_ = InGameInstance->GetStageInfoManager();
			DECLARE_TABLE();

			const uint16 SelectedChapterID = StageInfoManager_->GetSelectedChapter();
			UTBStoryMode_Chapter_Record* const ChapterInfoRecord = TableManager->storymode_chapter->Find(SelectedChapterID);
			if(IsValid(ChapterInfoRecord))
			{
				FName LevelName = GET_CM_RESOURCEUTILITY()->GetStageLevelPath(ChapterInfoRecord->IdleMode_Stage);
				if(false == LevelName.IsNone())
				{
					OutPath = LevelName;
					return true;
				}
			}

			return false;
		}
	}
};


UCMGameInstance::UCMGameInstance()
{
}

void UCMGameInstance::Init()
{
	Super::Init();
	
	InitManager();
	InitConsoleCommands();

///////////////////////////////////////////////////////////////////////////////////////////////
// ChunkDownloader
#if 0 < ENABLE_CHUNKDOWNLOADER
	InitPatchingSystem("Windows");
#endif //ENABLE_CHUNKDOWNLOADER
}

void UCMGameInstance::Shutdown()
{
	Super::Shutdown();

	if (IsValid(NetManager))
	{
		NetManager->ClearAll();
	}

///////////////////////////////////////////////////////////////////////////////////////////////
// ChunkDownloader
#if 0 < ENABLE_CHUNKDOWNLOADER
	FChunkDownloader::Shutdown();
#endif //ENABLE_CHUNKDOWNLOADER
}


void UCMGameInstance::FinishDestroy()
{
	Super::FinishDestroy();
	
	// if(NetworkObject != nullptr)
	// {
	// 	NetworkObject->NetClose();
	// }
}

void UCMGameInstance::OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld)
{
	if (IsValid(OldWorld))
	{
		CM_LOG(Log, "OldWorld : %s", *OldWorld->GetName());
	}

	if (IsValid(NewWorld))
	{
		CM_LOG(Log, "NewWorld : %s", *NewWorld->GetName());
		OnNewLevelLoaded.Broadcast();

		if( OnNewLevelLoadedOnce.IsBound())
		{
			OnNewLevelLoadedOnce.Broadcast();
			OnNewLevelLoadedOnce.Clear();
		}	
	}
}

void UCMGameInstance::Tick(float DeltaTime)
{
	if (NetManager != nullptr)
	{
		NetManager->Tick();
	}

	// if (NetworkObject != nullptr)
	// {
	// 	NetworkObject->NetTick(DeltaTime);
	// }
}

void UCMGameInstance::InitManager()
{
	NetManager = NewObject<UNetManager>(this);
	if (NetManager != nullptr) 
	{
		NetManager->Init(this);
	}
	else
	{
		CM_LOG_SCR("NetManager failed.");
	}
	
	TimeManager = NewObject<UCMTimeManager>(this);
	if (nullptr != TimeManager)
	{
		TimeManager->Init();
	}
	else
	{
		CM_LOG_SCR("NewObject TimeManager failed.");
	}

	ServerTimeManager = NewObject<UCMServerTimeManager>(this);
	if (nullptr != ServerTimeManager)
	{
		ServerTimeManager->Init();
	}
	else
	{
		CM_LOG_SCR("NewObject ServerTimeManager failed.");
	}

	AccountManager = NewObject<UCMAccountManager>(this);
	if (nullptr != AccountManager)
	{
		CM_LOG(Log, "Load Account!");
		AccountManager->LoadAccount();
		AccountManager->CreatePostManager();
		AccountManager->CreateAchieveManager();
		AccountManager->CreateSummonInfoManager();
		AccountManager->CreateCollectionInfoManager();
	}
	else
	{
		CM_LOG_SCR("NewObject AccountManager failed.");
	}

	ItemInfoManager = NewObject<UCMItemInfoManager>(this);
	if (ItemInfoManager != nullptr)
	{
		ItemInfoManager->Init();
	}
	else
	{
		CM_LOG_SCR("NewObject ItemInfoManager failed.");
	}

	CharInfoManager = NewObject<UCMCharInfoManager>(this);
	if (IsValid(CharInfoManager))
	{
		CharInfoManager->Init(ItemInfoManager);
	}
	else
	{
		CM_LOG_SCR("NewObject CharInfoManager failed.");
	}

	StageInfoManager = NewObject<UCMStageInfoManager>(this);
	if (nullptr == StageInfoManager)
	{
		CM_LOG_SCR("NewObject StageInfoManager failed.");
	}

	FacilityInfoManager = NewObject<UCMFacilityInfoManager>(this);
	if (nullptr == FacilityInfoManager)
	{	
		CM_LOG_SCR("NewObject FacilityInfoManager failed.");
	}
	else
	{
		FacilityInfoManager->Init();
	}

	TacticInfoManager = NewObject<UCMTacticInfoManager>(this);
	if (nullptr != TacticInfoManager)
	{
		//FacilityInfoManager->Init();		
	}
	else
	{
		CM_LOG_SCR("NewObject TacticInfoManager failed.");
	}

	GameActorManager = NewObject<UCMGameActorManager>(this);
	if( nullptr == GameActorManager)
	{
		CM_LOG_SCR("NewObject GameActorManager failed.");
	}

	//0531 apk 빌드로 임시 주석 처리
	SoundManager = NewObject<UCMSoundManager>(this);
	if (SoundManager == nullptr)
	{
		CM_LOG_SCR("NewObject SoundManager failed.");
	}
	SoundManager->InitSoundManager();

	UISystemManager = NewObject <UCMUISystemManager>(this);
	if (UISystemManager == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "CM - UCMGameInstance::Init() - UISystemManager failed");
	}

	TutorialManager = NewObject<UCMTutorialManager>(this);
	if(false == IsValid(TutorialManager))
	{
		CM_LOG_SCR("NewObject TutorialManager failed.");
	}
}

//void UCMGameInstance::InitCharStatFactory()
//{
//	if (IsValid(CharStatFactory) == false)
//	{
//		CharStatFactory = NewObject <UCMCharStatFactory>();
//		if (IsValid(CharStatFactory) == false)
//		{
//			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "CM - UCMGameInstance::Init() - UCMCharStatFactory failed");
//			return;
//		}
//
//		CharStatFactory->Init(this);
//	}
//}

//void UCMGameInstance::InitTextUtility()
//{
//	if (TextUtility == nullptr)
//	{
//		TextUtility = NewObject <UCMTextUtility>();
//		if (TextUtility == nullptr)
//		{
//			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "CM - UCMGameInstance::Init() - TextUtility failed");
//			return;
//		}
//
//		TextUtility->Init(this);
//	}
//}

void UCMGameInstance::SetCurrentGameMode(EGameMode eGameMode)
{
	CurrentGameMode = eGameMode;
}

bool UCMGameInstance::IsValidLevel(const FName& InLevelPath)
{
	// make sure the file exists if we are opening a local file
	FString LevelPath_ = InLevelPath.ToString();
	if (!GEngine->MakeSureMapNameIsValid(LevelPath_))
	{
		UE_LOG(LogLevel, Warning, TEXT("WARNING: The map '%s' does not exist."), *LevelPath_);
		return false;
	}

	return true;
}

void UCMGameInstance::LoadLevel_Complete()
{
	CM_LOG(Log, "");
	
	InternalOpenLevel();

	// 게임종료 후 로비로 돌아올 때만 UCMGameInstance::InternalOpenLevel 함수가 호출되지 않는다.
	// 주석처리하고 InternalOpenLevel();을 바로 호출한다.
	// FTimerHandle handle;
	// GetWorld()->GetTimerManager().SetTimer(handle, this, &UCMGameInstance::InternalOpenLevel, 0.01f, false);
}

void UCMGameInstance::InternalOpenLevel()
{
	CM_LOG(Log, "InternalOpenLevel %s", *LevelPath);
	
	UGameplayStatics::OpenLevel(GetWorld(), *LevelPath, true, LevelOptionString);
}

// void UCMGameInstance::OpenLevel(FName & InLevelPath, bool InIsRestorePageHistory, bool IsAsyncLoad, TArray<TTuple<FString, FString>> InLevelOptions, EUI_PanelTransitionType In_E_PanelTransitionType, const bool IsStorePageHistory_From_CurrentLevel)
// {
// 	CM_LOG(Log, "OpenLevel: %s", *InLevelPath.ToString());
//
// 	// 모바일에서는, 가비지 컬렉션를 진행한다
// #ifdef PLATFORM_ANDROID
// 	GEngine->ForceGarbageCollection(true);
// #endif
//
// 	if(false == IsValidLevel(InLevelPath))
// 	{
// 		UISystemManager->Open_ConfirmPopup(TEXT("경고"), FString::Printf(TEXT("%s 레벨이 존재하지 않습니다."), *InLevelPath.ToString()));
// 		return;
// 	}
//
// 	if(IsValid(UISystemManager))
// 	{
// 		// 현재 게임모드의 LevelName을 키로하여 페이지 히스토리 저장
// 		FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);
//
// 		if(IsStorePageHistory_From_CurrentLevel)
// 		{
// 			// 현재 레벨의 페이지 히스토리 저장
// 			UISystemManager->StorePageHistory(CurrentLevelName);
// 		}
// 		// // 모든 패널 제거
// 		// UISystemManager->RemoveAllPanels();
//
// 		LevelPath = InLevelPath.ToString();
// 		MapToLoad = FSoftObjectPath(InLevelPath);
//
// 		// Add LevelOption
// 		LevelOption.Empty();
// 		{
// 			FString CurrentLevelPath;
// 			FCMLevelInfo LevelInfo;
// 			if(UISystemManager->TryGetCurrentLevelInfo(LevelInfo))
// 			{
// 				CurrentLevelPath = LevelInfo.LevelPath;
// 			}
// 			// if(UISystemManager->TryGetLevelInfo_By_LevelName(LevelInfo, CurrentLevelName))
// 			// {
// 			// 	CurrentLevelPath = LevelInfo.LevelPath;
// 			// }
// 			
// 			// PrevLevelName : 새로 열릴 레벨 기준으로 이전 레벨 이름
// 			LevelOption.Append(TEXT("?"));
// 			LevelOption.Append(FCMLevelInfo::LevelOptionKey_PrevLevelName);
// 			LevelOption.Append(TEXT("="));
// 			LevelOption.Append(CurrentLevelName);
// 			
// 			// PrevLevelName : 새로 열릴 레벨 기준으로 이전 레벨 경로
// 			LevelOption.Append(TEXT("?"));
// 			LevelOption.Append(FCMLevelInfo::LevelOptionKey_PrevLevelPath);
// 			LevelOption.Append(TEXT("="));
// 			LevelOption.Append(CurrentLevelPath);
// 			
// 			// CurrentLevelPath : 새로 열릴 레벨 경로
// 			LevelOption.Append(TEXT("?"));
// 			LevelOption.Append(FCMLevelInfo::LevelOptionKey_CurrentLevelPath);
// 			LevelOption.Append(TEXT("="));
// 			LevelOption.Append(InLevelPath.ToString());
// 			
// 			// RestorePageHistory
// 			LevelOption.Append(TEXT("?"));
// 			LevelOption.Append(FCMLevelInfo::LevelOptionKey_IsRestorePageHistory);
// 			LevelOption.Append(TEXT("="));
// 			if (InIsRestorePageHistory)
// 			{
// 				LevelOption.Append(FCMLevelInfo::LevelOptionValue_True);
// 			}
//
// 			for (TTuple<FString, FString> _LevelOption : InLevelOptions)
// 			{
// 				LevelOption.Append(TEXT("?"));
// 				LevelOption.Append(_LevelOption.Get<0>());
// 				LevelOption.Append(TEXT("="));
// 				LevelOption.Append(_LevelOption.Get<1>());
// 			}
// 		}
// 		
// 		UCMPageInfo* const PanelInfo = UCMPageInfo::Create(this, EUI_PanelType::None, NAME_None, false, EUI_PrevPanelManipulateType::RemoveAll_Forcibly, In_E_PanelTransitionType, false);
// 		PanelInfo->AddPanelData_As_OpenLevel_Callback(PanelDataKeys::CB_Transition_In_For_Level_Open, [this, IsAsyncLoad](TFunction<void()> cb) -> bool
// 		{
// 			// 레벨 이동 시에는 모든 패널 제거
// 			UISystemManager->RemoveAllPanels();
// 			
// 			if (IsAsyncLoad)
// 			{
// 				Loader.RequestAsyncLoad(MapToLoad, FStreamableDelegate::CreateUObject(this, &UCMGameInstance::LoadLevel_Complete));
// 			}
// 			else
// 			{
// 				InternalOpenLevel();
// 			}
//
// 			return false;
// 		});
// 		UISystemManager->AddPanel(PanelInfo);
// 	}
//
// // #ifdef ASYNC_LOAD
// //
// // 	switch ( CurrentGameMode )
// // 	{
// // 		case EGameMode::LOGIN:
// // 		case EGameMode::LOBBY:
// // 		{
// // 			//LevelName = FString("/Game/Game/Level/") + level.ToString();
// // 			//CM_LOG(Log, "LevelName %s", *LevelName);
// // 			LevelName = level.ToString();
// //
// // 			MapToLoad = FSoftObjectPath(level);
// // 			Loader.RequestAsyncLoad(MapToLoad, FStreamableDelegate::CreateUObject(this, &UCMGameInstance::LoadCompleteLevel));
// // 		}
// // 		break;
// // 		case EGameMode::GAME:
// // 		{
// // 			CM_LOG(Log, "level %s", *level.ToString());
// // 			UGameplayStatics::OpenLevel(GetWorld(), level);
// // 		}
// // 		break;
// // 	}
// // 	
// // #else
// //
// // 	CM_LOG(Log, "level %s", *level.ToString());
// // 	UGameplayStatics::OpenLevel(GetWorld(), level);
// //
// // #endif
// }

void UCMGameInstance::OpenLevel(FName& InLevelPath, bool InIsRestorePageHistory, bool IsAsyncLoad, TMap<FString, FString> InLevelOptions, EUI_PanelTransitionType In_E_PanelTransitionType, const bool IsStorePageHistory_From_CurrentLevel)
{
	CM_LOG(Log, "OpenLevel: %s", *InLevelPath.ToString());

	// 모바일에서는, 가비지 컬렉션를 진행한다
#ifdef PLATFORM_ANDROID
	GEngine->ForceGarbageCollection(true);
#endif

	if(false == IsValidLevel(InLevelPath))
	{
		UISystemManager->Open_ConfirmPopup(TEXT("경고"), FString::Printf(TEXT("%s 레벨이 존재하지 않습니다."), *InLevelPath.ToString()));
		return;
	}

	if(IsValid(UISystemManager))
	{
		// 현재 게임모드의 LevelName을 키로하여 페이지 히스토리 저장
		FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);

		if(IsStorePageHistory_From_CurrentLevel)
		{
			// 현재 레벨의 페이지 히스토리 저장
			UISystemManager->StorePageHistory(CurrentLevelName);
		}
		// // 모든 패널 제거
		// UISystemManager->RemoveAllPanels();

		LevelPath = InLevelPath.ToString();
		MapToLoad = FSoftObjectPath(InLevelPath);

		// Add LevelOption
		{
			TMap<FString, FString> LevelOptions = InLevelOptions;
			
			LevelOptionString.Empty();
			
			FString CurrentLevelPath;
			FCMLevelInfo LevelInfo;
			if(UISystemManager->TryGetCurrentLevelInfo(LevelInfo))
			{
				CurrentLevelPath = LevelInfo.LevelPath;
			}
			// if(UISystemManager->TryGetLevelInfo_By_LevelName(LevelInfo, CurrentLevelName))
			// {
			// 	CurrentLevelPath = LevelInfo.LevelPath;
			// }

			// PrevLevelName : 새로 열릴 레벨 기준으로 이전 레벨 이름
			if(LevelOptions.Contains(FCMLevelInfo::LevelOptionKey_PrevLevelName))
			{
				LevelOptions[FCMLevelInfo::LevelOptionKey_PrevLevelName] = CurrentLevelName;
			}
			else
			{
				LevelOptions.Emplace(FCMLevelInfo::LevelOptionKey_PrevLevelName, CurrentLevelName);
			}

			// PrevLevelName : 새로 열릴 레벨 기준으로 이전 레벨 경로
			if(LevelOptions.Contains(FCMLevelInfo::LevelOptionKey_PrevLevelPath))
			{
				LevelOptions[FCMLevelInfo::LevelOptionKey_PrevLevelPath] = CurrentLevelPath;
			}
			else
			{
				LevelOptions.Emplace(FCMLevelInfo::LevelOptionKey_PrevLevelPath, CurrentLevelPath);
			}

			// CurrentLevelPath : 새로 열릴 레벨 경로
			if(LevelOptions.Contains(FCMLevelInfo::LevelOptionKey_CurrentLevelPath))
			{
				LevelOptions[FCMLevelInfo::LevelOptionKey_CurrentLevelPath] = InLevelPath.ToString();
			}
			else
			{
				LevelOptions.Emplace(FCMLevelInfo::LevelOptionKey_CurrentLevelPath, InLevelPath.ToString());
			}

			// RestorePageHistory
			if(LevelOptions.Contains(FCMLevelInfo::LevelOptionKey_IsRestorePageHistory))
			{
				if (InIsRestorePageHistory)
				{
					LevelOptions[FCMLevelInfo::LevelOptionKey_IsRestorePageHistory] = FCMLevelInfo::LevelOptionValue_True;
				}
			}
			else
			{
				if (InIsRestorePageHistory)
				{
					LevelOptions.Emplace(FCMLevelInfo::LevelOptionKey_IsRestorePageHistory, FCMLevelInfo::LevelOptionValue_True);
				}
			}
			
			// // PrevLevelName : 새로 열릴 레벨 기준으로 이전 레벨 이름
			// if(LevelOptions.Contains(FCMLevelInfo::LevelOptionKey_PrevLevelName))
			// {
			// 	LevelOptionString.Append(TEXT("?"));
			// 	LevelOptionString.Append(FCMLevelInfo::LevelOptionKey_PrevLevelName);
			// 	LevelOptionString.Append(TEXT("="));
			// 	LevelOptionString.Append(CurrentLevelName);
			// }
			
			// // PrevLevelName : 새로 열릴 레벨 기준으로 이전 레벨 경로
			// if(false == LevelOptions.Contains(FCMLevelInfo::LevelOptionKey_PrevLevelPath))
			// {
			// 	LevelOptionString.Append(TEXT("?"));
			// 	LevelOptionString.Append(FCMLevelInfo::LevelOptionKey_PrevLevelPath);
			// 	LevelOptionString.Append(TEXT("="));
			// 	LevelOptionString.Append(CurrentLevelPath);
			// }
			
			// // CurrentLevelPath : 새로 열릴 레벨 경로
			// if(false == LevelOptions.Contains(FCMLevelInfo::LevelOptionKey_CurrentLevelPath))
			// {
			// 	LevelOptionString.Append(TEXT("?"));
			// 	LevelOptionString.Append(FCMLevelInfo::LevelOptionKey_CurrentLevelPath);
			// 	LevelOptionString.Append(TEXT("="));
			// 	LevelOptionString.Append(InLevelPath.ToString());
			// }
			
			// // RestorePageHistory
			// if(false == LevelOptions.Contains(FCMLevelInfo::LevelOptionKey_IsRestorePageHistory))
			// {
			// 	LevelOptionString.Append(TEXT("?"));
			// 	LevelOptionString.Append(FCMLevelInfo::LevelOptionKey_IsRestorePageHistory);
			// 	LevelOptionString.Append(TEXT("="));
			// 	if (InIsRestorePageHistory)
			// 	{
			// 		LevelOptionString.Append(FCMLevelInfo::LevelOptionValue_True);
			// 	}
			// }

			LevelOptionString.Empty();
			for (TPair<FString, FString> Kvp : LevelOptions)
			{
				LevelOptionString.Append(TEXT("?"));
				LevelOptionString.Append(Kvp.Key);
				LevelOptionString.Append(TEXT("="));
				LevelOptionString.Append(Kvp.Value);
				
				// LevelOption.Append(TEXT("?"));
				// LevelOption.Append(_LevelOption.Get<0>());
				// LevelOption.Append(TEXT("="));
				// LevelOption.Append(_LevelOption.Get<1>());
			}
		}
		
		UCMPageInfo* const PanelInfo = UCMPageInfo::Create(this, EUI_PanelType::None, NAME_None, false, EUI_PrevPanelManipulateType::RemoveAll_Forcibly, In_E_PanelTransitionType, false);
		PanelInfo->AddPanelData_As_OpenLevel_Callback(PanelDataKeys::CB_Transition_In_For_Level_Open, [this, IsAsyncLoad](TFunction<void()> cb) -> bool
		{
			// 레벨 이동 시에는 모든 패널 제거
			UISystemManager->RemoveAllPanels();
			
			if (IsAsyncLoad)
			{
				Loader.RequestAsyncLoad(MapToLoad, FStreamableDelegate::CreateUObject(this, &UCMGameInstance::LoadLevel_Complete));
			}
			else
			{
				InternalOpenLevel();
			}

			return false;
		});
		UISystemManager->AddPanel(PanelInfo);
	}
}

void UCMGameInstance::OpenLevel_By_GameModeType(EUI_GameModeType In_EUI_GameModeType, TMap<FString, FString> InLevelOptions, bool InIsRestorePageHistory, bool IsAsyncLoad, EUI_PanelTransitionType In_E_PanelTransitionType, const bool IsStorePageHistory_From_CurrentLevel)
{
	UCMDataAssetManager* DataAssetManager = GET_CM_DATAASSET();
	UUISystemData* UISystemData = DataAssetManager->GetUISystemData();
	if (IsValid(UISystemData))
	{
		FGameModeInfo GameModeInfo;
		if (UISystemData->TryGetGameModeInfo_By_GameModeType(GameModeInfo, In_EUI_GameModeType))
		{
			FName Path = *GameModeInfo.LevelAsset.GetLongPackageName();
			
			// 기본 레벨 패스에 대한 추가 가공 함수 호출
			if (OpenLevelPathPrevHandlers.Contains(In_EUI_GameModeType) && OpenLevelPathPrevHandlers[In_EUI_GameModeType](Path, InLevelOptions, this))
			{
				CM_LOG(Log, "OpenLevelPath Changed : %s", *Path.ToString());
			}
			
			//FString a = *GameModeInfo.LevelAsset.ToString();
			//FString b = *GameModeInfo.LevelAsset.GetAssetName();
			//FString c = *GameModeInfo.LevelAsset.GetLongPackageName();
			//FString d = *Path.ToString();
			//CM_LOG(Log, "%s, %s, %s, %s", *a, *b, *c, *d);
			OpenLevel(Path, InIsRestorePageHistory, IsAsyncLoad, InLevelOptions, In_E_PanelTransitionType, IsStorePageHistory_From_CurrentLevel);
		}
	}
}

void UCMGameInstance::OpenPrevLevel_By_GameModeClass(UClass* InGameModeClass, TMap<FString, FString> InLevelOptions, bool InIsRestorePageHistory, bool IsAsyncLoad, EUI_PanelTransitionType In_E_PanelTransitionType)
{
	UCMDataAssetManager* DataAssetManager = GET_CM_DATAASSET();
	UUISystemData* UISystemData = DataAssetManager->GetUISystemData();
	if (IsValid(UISystemData))
	{
		EUI_GameModeType E_GameModeType;
		FGameModeInfo GameModeInfo;
		if (UISystemData->TryGetGameModeInfo_By_GameModeClass(GameModeInfo, E_GameModeType, InGameModeClass))
		{
			FGameModeInfo OutPrevLevelInfo;
			if (UISystemData->TryGetGameModeInfo_By_GameModeType(OutPrevLevelInfo, GameModeInfo.E_PrevGameModeType))
			{
				FName Path = *OutPrevLevelInfo.LevelAsset.GetLongPackageName();
				TMap<FString, FString> LevelOptions = InLevelOptions;
				// TArray<TTuple<FString, FString>> LevelOptions = InLevelOptions;
				// 기본 레벨 패스에 대한 추가 가공 함수 호출 // todo #refact 중복 제거
				if (OpenLevelPathPrevHandlers.Contains(GameModeInfo.E_PrevGameModeType) &&
					OpenLevelPathPrevHandlers[GameModeInfo.E_PrevGameModeType](Path, LevelOptions, this))
				{
					CM_LOG(Log, "OpenLevelPath Changed : %s", *Path.ToString());
				}
				
				OpenLevel(Path, InIsRestorePageHistory, IsAsyncLoad, LevelOptions, In_E_PanelTransitionType, false);
			};
		}
	}
}

void UCMGameInstance::LoadStreamLevel(FName & level)
{
	if (!level.GetStringLength())
	{
		return;
	}

	ULevelStreaming* const Streamlevel = UGameplayStatics::GetStreamingLevel(GetWorld(), level);

	if (Streamlevel == nullptr || Streamlevel->IsLevelVisible())
	{
		return;
	}
	else
	{
		FLatentActionInfo info;
		info.CallbackTarget = this;
		info.ExecutionFunction = FName("SuccessLdStreamLevel");
		info.UUID = 0;
		info.Linkage = 0;

		UGameplayStatics::LoadStreamLevel(GetWorld(), level, true, true, info);
	}
}
void UCMGameInstance::SuccessLdStreamLevel()
{
}

void UCMGameInstance::UnLoadStreamLevel(FName & level)
{
	if (!level.GetStringLength())
	{
		return;
	}

	ULevelStreaming* const Streamlevel = UGameplayStatics::GetStreamingLevel(GetWorld(), level);

	if (Streamlevel == nullptr)
	{
		return;
	}

	if (Streamlevel->IsLevelVisible())
	{
		FLatentActionInfo info;
		info.CallbackTarget = this;
		info.ExecutionFunction = FName("SuccessUlStreamLevel");
		info.UUID = 0;
		info.Linkage = 0;

		UGameplayStatics::UnloadStreamLevel(GetWorld(), level, info, true);
	}
	else
	{
		CM_LOG_S(Error);
	}
}
void UCMGameInstance::SuccessUlStreamLevel() 
{
}

bool UCMGameInstance::GetIsLoadStreamLevel( FName& level )
{
	if (!level.GetStringLength())
	{
		return false;
	}

	ULevelStreaming* const Streamlevel = UGameplayStatics::GetStreamingLevel(GetWorld(), level);
	if (Streamlevel == nullptr)
	{
		return false;
	}
	
	if (Streamlevel->IsLevelVisible())
	{
		return true;
	}
	return false;
}

bool UCMGameInstance::IsStreamLevel(FName& level)
{
	ULevelStreaming* const Streamlevel = UGameplayStatics::GetStreamingLevel(GetWorld(), level);
	if (Streamlevel == nullptr)
	{
		return false;
	}
	return true;
}

void UCMGameInstance::QuitGame() // 게임종료
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
}

APlayerController* UCMGameInstance::GetFirstLocalPlayerController(UWorld* World) const // 컨트롤러
{
	if (World == nullptr)
	{
		for (ULocalPlayer* Player : LocalPlayers)
		{
			if (Player && Player->PlayerController)
			{
				return Player->PlayerController;
			}
		}
	}
	else
	{
		for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			if (*Iterator != nullptr && (*Iterator)->IsLocalController())
			{
				return Iterator->Get();
			}
		}
	}
	return nullptr;
}


void UCMGameInstance::InitConsoleCommands()
{
	UCMConsoleCommands::Init();
	// ConsoleCommands = NewObject<UCMConsoleCommands>(this);
	// ConsoleCommands->Init();
}

void UCMGameInstance::CheckPlayStandAlone()
{
	if (CMGLOBAL::CheckFileExist(TEXT("StandAlone.cfg")))
	{
		PlayStandAlone = true;
		return;
	}
}

void UCMGameInstance::OpenLevel_To_SkillPriview(uint32 CharID, ETB_Skill_Slot In_E_Skill_Slot)
{
	OpenLevel_By_GameModeType(
		EUI_GameModeType::Lobby_SkillPreview,
		{
			TTuple<FString, FString>(FString(TEXT("CharID")), FString::FromInt(CharID)),
			TTuple<FString, FString>(FString(TEXT("E_Skill_Slot")), FString::FromInt(static_cast<int32>(In_E_Skill_Slot))),
		});
}

const bool UCMGameInstance::HasLevelOption(const AGameMode* const InGameMode, const FString InKey)
{
	if(IsValid(InGameMode))
	{
		return UGameplayStatics::HasOption(InGameMode->OptionsString, InKey);		
	}

	return false;
}

const FString UCMGameInstance::GetLevelOptionValue(const AGameMode* const InGameMode, const FString InKey)
{
	if(IsValid(InGameMode))
	{
		return UGameplayStatics::ParseOption(InGameMode->OptionsString, InKey);		
	}

	return CMConstant::G_Empty_FString;
}


///////////////////////////////////////////////////////////////////////////////////////////////
// ChunkDownloader
void UCMGameInstance::InitPatchingSystem(const FString& InPatchPlatform)
{
	PatchPlatform = InPatchPlatform;

	FHttpModule& Http = FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http.CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UCMGameInstance::OnPatchingSystemVersionResponse);

	Request->SetURL(PatchVersionURL);
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->ProcessRequest();
}

void UCMGameInstance::OnPatchingSystemVersionResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FString ContentBuildId = Response->GetContentAsString();
	InitChunkDownloader(ContentBuildId);
}

void UCMGameInstance::InitChunkDownloader(const FString& BuildID)
{
	const FString DeploymentName = "PatchingDemoLive";

	TSharedRef<FChunkDownloader> Downloader = FChunkDownloader::GetOrCreate();
	Downloader->Initialize("Windows", 8);
	Downloader->LoadCachedBuild(DeploymentName);
	TFunction<void(bool bSuccess)> UpdateCompleteCallback = [&](bool bSuccess) 
	{ 
		OnManifestUpdateComplete(bSuccess);
	};
	Downloader->UpdateBuild(DeploymentName, BuildID, UpdateCompleteCallback);
}

void UCMGameInstance::OnManifestUpdateComplete(const bool bSuccess)
{
	bIsDownloadManifestUpToDate = bSuccess;
	PatchGame();
}

void UCMGameInstance::OnDownloadComplete(const bool bSuccess)
{
	if (bSuccess)
	{
		TSharedRef<FChunkDownloader> Downloader = FChunkDownloader::GetChecked();
		FJsonSerializableArrayInt DownloadedChunks;
		for (int32 ChunkID : ChunkDownloadList)
		{
			DownloadedChunks.Add(ChunkID);
		}
		TFunction<void(const bool bSuccess)> MountCompleteCallback = [&](const bool bSuccess) { OnMountComplete(bSuccess); };
		Downloader->MountChunks(DownloadedChunks, MountCompleteCallback);
		OnPatchComplete.Broadcast(true);
	}
	else
	{
		OnPatchComplete.Broadcast(false);
	}
}

void UCMGameInstance::OnLoadingModeComplete(const bool bSuccess)
{
	OnDownloadComplete(bSuccess);
}

void UCMGameInstance::OnMountComplete(const bool bSuccess)
{
	OnPatchComplete.Broadcast(bSuccess);
}

void UCMGameInstance::GetLoadingProgress(int32& BytesDownloaded, int32& TotalBytesToDownload, float& DownloadPercent, int32& ChunkMounted, int32& TotalChunksToMount, float& MountPercent) const
{
	TSharedRef<FChunkDownloader> Downloader = FChunkDownloader::GetChecked();
	FChunkDownloader::FStats LoadingStats = Downloader->GetLoadingStats();

	BytesDownloaded = LoadingStats.BytesDownloaded;
	TotalBytesToDownload = LoadingStats.TotalBytesToDownload;

	ChunkMounted = LoadingStats.ChunksMounted;
	TotalChunksToMount = LoadingStats.TotalChunksToMount;

	DownloadPercent = ((float)BytesDownloaded / (float)TotalBytesToDownload) * 100.f;
	MountPercent = ((float)ChunkMounted / (float)TotalChunksToMount) * 100.f;
}

bool UCMGameInstance::PatchGame()
{
	if (bIsDownloadManifestUpToDate)
	{
		TSharedRef<FChunkDownloader> Downloader = FChunkDownloader::GetChecked();
		for (int32 ChunkID : ChunkDownloadList)
		{
			int32 ChunkStatus = static_cast<int32>(Downloader->GetChunkStatus(ChunkID));
		}

#if 1
		TFunction<void(const bool bSuccess)> DownloadCompleteCallback = [&](const bool bSuccess) { OnDownloadComplete(bSuccess); };
		Downloader->DownloadChunks(ChunkDownloadList, DownloadCompleteCallback, 1);

		TFunction<void(const bool bSuccess)> LoadingModeCompleteCallback = [&](const bool bSuccess) { OnLoadingModeComplete(bSuccess); };
		Downloader->BeginLoadingMode(LoadingModeCompleteCallback);
#else
		TFunction<void(bool)> MountCompleteCallback = [&](const bool bSuccess) { bSuccess ? OnPatchComplete.Broadcast(true) : OnPatchComplete.Broadcast(false); };
		Downloader->MountChunks(ChunkDownloadList, MountCompleteCallback);
#endif
		return true;
	}
	return false;
}

FPPatchStats UCMGameInstance::GetPatchStatus()
{
	TSharedRef<FChunkDownloader> Downloader = FChunkDownloader::GetChecked();
	FChunkDownloader::FStats LoadingStats = Downloader->GetLoadingStats();

	FPPatchStats RetStats;
	RetStats.FilesDownloaded = LoadingStats.TotalFilesToDownload;
	RetStats.MBDownloaded = (int32)(LoadingStats.BytesDownloaded / (1024 * 1024));
	RetStats.TotalMBToDownload = (int32)(LoadingStats.TotalBytesToDownload / (1024 * 1024));
	RetStats.DownloadPercent = (float)LoadingStats.BytesDownloaded / (float)LoadingStats.TotalBytesToDownload;
	RetStats.LastError = LoadingStats.LastError;

	return RetStats;
}

void UCMGameInstance::GetDiskTotalAndFreeSpace(const FString& InPath, uint64& TotalSpace, uint64& FreeSpace)
{
	uint64 TotalDiskSpace = 0;
	uint64 TotalDiskFreeSpace = 0;
	if (FPlatformMisc::GetDiskTotalAndFreeSpace(InPath, TotalDiskSpace, TotalDiskFreeSpace))
	{
		TotalSpace = TotalDiskFreeSpace;
		FreeSpace = FUnitConversion::Convert(TotalSpace, EUnit::Bytes, EUnit::Megabytes);
		return;
	}
	TotalSpace = TotalDiskSpace;
	FreeSpace = TotalDiskFreeSpace;
}