// Fill out your copyright notice in the Description page of Project Settings.


#include "Test_StageGameMode.h"

#include "CMBattleMonsterAIController.h"
#include "CMTestSceneMode.h"
#include "CMLoginController.h"
#include "CMBattlePlayerAIController.h"
#include "CMCharInfoManager.h"
#include "CMGameSingleton.h"
#include "CMMonsterPawn.h"
#include "TBManager.h"
#include "CMPlayerManager.h"
#include "CMLevelDataManager.h"
#include "CMStageInfoManager.h"
#include "CMTacticInfoManager.h"
#include "CMGameInstance.h"
#include "CMPageInfo.h"
#include "CMUISystemManager.h"
#include "StageClear/CMStageClearRuler.h"
#include "CMCommon.h"
#include "CMCutSceneManager.h"
#include "CMDamageTextManager.h"
#include "CMDataAssetManager.h"
#include "CMStageLoader.h"
#include "CMTextUtility.h"
#include "CMTimeFormatter.h"
#include "CMTimeManager.h"
#include "UITextKeyData.h"
#include "Common/CMEditorCommon.h"
#include "Internationalization/Regex.h"
#include "StageClear/CMStageRecorder.h"
#include "UIStoryGameModePage.h"
#include "CMInGameController.h"
#include "CMTableInfoManager.h"
#include "CMCharStatFactory.h"

#include "CMSoundManager.h"
#include "CMAssetManager.h"
#include "CMPanelDataBase.h"
#include "CMPlayerPawn.h"
#include "CMStatComponent.h"
#include "Engine/StreamableManager.h"
#include "Test/WITH_EDITOR_TestSetting.h"


ATest_StageGameMode::ATest_StageGameMode()
{
	PlayerControllerClass = ACMLoginController::StaticClass();
}

void ATest_StageGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	// 테스트 레벨은 디버그그리기 기본 활성화
	UWITH_EDITOR_TestSetting::WITH_EDITOR_DrawDebug_Battle = true;
	
	GET_CM_SINGLETON()->InitManagerRelatedTable();
	
	// test용 설정 데이터 로드 및 구성
	InitTestData();
	
	Super::InitGame(MapName, Options, ErrorMessage);

	// DateTime Format Test
	FDateTime DateTime = FDateTime::UtcNow();
	FString DateString = DateTime.ToString(TEXT("%y %Y 년 %m 월 %d %D 일 %h %H 시 %M 분 %S %s 초"));
	CM_LOG(Log, "%s", *DateString);
}

void ATest_StageGameMode::InitGameState()
{
	Super::InitGameState();

	UCMGameInstance* GameInstance = Cast<UCMGameInstance>(GetGameInstance());
	if (IsValid(GameInstance))
	{
		if (IsValid(GameInstance->GetSoundManager()))
		{
			//0531 apk 빌드로 임시 주석 처리
			//GameInstance->GetSoundManager()->PlaySound(1001001);
		}
	}
}

void ATest_StageGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();

	//
	// // UI 초기화
	CM_VALID_RETURN(UISystemManager);
	// UISystemManager->Init(GameInstance);
	// UISystemManager->InitPageHistory();
	// UISystemManager->RestorePages();
	//
	if (IsValid(UISystemManager))
	{
		// main page
		{
			// UCMPageInfo* PageInfo = UCMPageInfo::Create(this, EUI_PanelType::StoryGameModePage, NAME_None, false,
			// 	EUI_PrevPanelManipulateType::RemoveAll_Forcibly,
			// 	EUI_PanelTransitionType::None, false);
			// PageInfo->UIPanelContainerType = EUI_PanelContainerType::Page;
			// PageInfo->IsShowBackButton = false;
			// UISystemManager->AddPanel(PageInfo);

			UISystemManager->AddPage(EUI_PanelType::StoryGameModePage, [this](UCMPanelInfoBase* InPanelInfo)
			{
				InPanelInfo->AddPanelData_As_Callback(PanelDataKeys::CB_Transition_In_For_Panel_Added, [this](UCMPanelBase* InPanel)
				{
					UUIStoryGameModePage* const StoryGameModePage = Cast<UUIStoryGameModePage>(InPanel);
					if (IsValid(StoryGameModePage))
					{
						// StoryGameModePage->InitAll();
						// StoryGameModePage->PlayStartAnimation();
						// StoryGameModePage->SetMinimapSync(true);
						// StoryGameModePage->SetPlayerSlotActionMode(true);
						StoryGameModePage->SetMinimapVisibility(false);
					}
				});
			});
		}
	
		// test page
		{
			UCMPageInfo* PageInfo = UCMPageInfo::Create(this, EUI_PanelType::CharTestMainPage, NAME_None, false,
				EUI_PrevPanelManipulateType::Show,
				EUI_PanelTransitionType::None, false);
			PageInfo->UIPanelContainerType = EUI_PanelContainerType::PageOver;
			PageInfo->IsShowBackButton = false;
			PageInfo->AddPanelData_As_Bool(TEXT("AllPlayerPawnAI_Check_Visible"), true);
			PageInfo->AddPanelData_As_Bool(TEXT("AllMonsterPawnAI_Check_Visible"), true);
			UISystemManager->AddPanel(PageInfo);
		}
	}
	//
	// if (false == IsInitTestData)
	// {
	// 	UISystemManager->Open_ConfirmPopup(TEXT(""), TEXT("Test_StageData_Default 파일을 복사하여 Test_StageData 파일을 생성해주세요."));
	// }
	//
	// // StageInfo 테이블을 통해 스테이지를 구성
	// UCMStageLoader* StageLoader = NewObject<UCMStageLoader>();
	// if (false == StageLoader->OnStageLoaderStepChanged.IsBoundToObject(this))
	// {
	// 	StageLoader->OnStageLoaderStepChanged.BindWeakLambda(this, [this](ECM_StageLoadStep In_E_StageLoadStep, bool InIsSuccess)
	// 	{
	// 		if (ECM_StageLoadStep::Done == In_E_StageLoadStep)
	// 		{
	// 			// 모든 준비가 끝났다. Match 시작 -> HandleMatchHasStarted() 호출됨.
	// 			StartMatch();
	// 		}
	// 	});
	// }
	// StageLoader->Init(GetWorld(), LevelDataManager, PlayerManager, StageData->StageID, StageData->StageID);
	// StageLoader->Start();
}

void ATest_StageGameMode::HandleMatchHasStarted()
{
	// Match 시작 됨
	// StartMatch()를 명시적 호출 이 후 호출(BeginPlay() 이 후 시점)
	Super::HandleMatchHasStarted();

	if (true)
	{
		//FTB_Char_UltiGuage Char_UltiGuage;
		//Char_UltiGuage.Max = 32767;

		//FCMCharStatInfo CharStatInfo;
		//CharStatInfo.SetUltiGauge(Char_UltiGuage);

		// UCMPlayerManager* PlayerManager = GetPlayerManager();
		// for (auto KVP : PlayerManager->GetPlayerMap())
		// {
		// 	UCMCharBattleComponent* CharBattleComponent = KVP.Value->GetBattleComponent();
		// 	UCMStatComponent* StatComponent = KVP.Value->GetStatComponent();
		// 	StatComponent->SetUltiGauge(StageData->DefaultUltiGauge);
		// 	//StatComponent->AddCharStatInfo_ToAdd(StageData->CharStatInfoAdd);
		// }
	}
}

void ATest_StageGameMode::InitStageClearRuler()
{
	// Super::InitStageClearRuler();
	
	TArray<FTB_Stage_ClearRule> StageClearConditions;
	TArray<FTB_Stage_ClearRule> StageDefeatConditions;
	
	auto ConditionSetting = [](TArray<FTB_Stage_ClearRule>& OutConditions, const TArray<FTest_Stage_ClearRule>& InConditions)
	{
		for (const FTest_Stage_ClearRule& Test_Stage_ClearRule : InConditions)
		{
			OutConditions.Add(FTB_Stage_ClearRule());
			FTB_Stage_ClearRule& F_Stage_ClearRule = OutConditions.Last();
			F_Stage_ClearRule.Type = Test_Stage_ClearRule.Type;
			F_Stage_ClearRule.Value = Test_Stage_ClearRule.Count;
		}
	};
	
	ConditionSetting(StageClearConditions, StageData->Test_StageClearConditions);
	ConditionSetting(StageDefeatConditions, StageData->Test_StageDefeatConditions);

	if(IsValid(StageClearRuler))
	{
		if (false == StageClearRuler->OnStageClearRulerChanged.IsBoundToObject(this))
		{
			StageClearRuler->OnStageClearRulerChanged.BindWeakLambda(this, [this]()
			{
				const ECM_StageClearConditionResult E_StageClearConditionResult = StageClearRuler->GetStageClearConditionResult();
				if (StageClearRuler->IsDone())
				{
					StageClearRuler->PrintAllStageClearConditions();
					const EStageResultType E_StageResultType = GetStageResult_By_StageClearRuler();
					GoGameEnd(E_StageResultType);
				}
			});
		}
		// const uint32 StageID = StageInfoManager->GetSelectedStageID();
		// StageClearRuler->Init_By_StageId(StageID, this, M_StageRecorder);
		StageClearRuler->Init(this, StageRecorder, StageClearConditions, StageData->Test_ClearConditionChain ? 1 : 0, StageDefeatConditions);
		StageClearRuler->Update_Event();
	}
}

void ATest_StageGameMode::StartPlay()
{
	// InitGameState() 이 후 호출
	Super::StartPlay();

	// // stage clear ruler
	// if (false == IsValid(StageClearRuler))
	// {
	// 	TArray<FTB_Stage_ClearRule> StageClearConditions;
	// 	TArray<FTB_Stage_ClearRule> StageDefeatConditions;
	//
	// 	auto ConditionSetting = [](TArray<FTB_Stage_ClearRule>& OutConditions, const TArray<FTest_Stage_ClearRule>& InConditions)
	// 	{
	// 		for (const FTest_Stage_ClearRule& Test_Stage_ClearRule : InConditions)
	// 		{
	// 			OutConditions.Add(FTB_Stage_ClearRule());
	// 			FTB_Stage_ClearRule& F_Stage_ClearRule = OutConditions.Last();
	// 			F_Stage_ClearRule.Type = Test_Stage_ClearRule.Type;
	// 			F_Stage_ClearRule.Value = Test_Stage_ClearRule.Count;
	// 		}
	// 	};
	//
	// 	ConditionSetting(StageClearConditions, Test_StageClearConditions);
	// 	ConditionSetting(StageDefeatConditions, Test_StageDefeatConditions);
	//
	// 	StageClearRuler = NewObject<UCMStageClearRuler>();
	// 	if (false == StageClearRuler->OnStageClearRulerChanged.IsBoundToObject(this))
	// 	{
	// 		StageClearRuler->OnStageClearRulerChanged.BindWeakLambda(this, [this]()
	// 		{
	// 			const ECM_StageClearConditionResult E_StageClearConditionResult = StageClearRuler->GetStageClearConditionResult();
	// 			CM_LOG(Log, "E_StageClearConditionResult : %s(%d)", *CMUtils::EnumToFString(TEXT("ECM_StageClearConditionResult"), E_StageClearConditionResult), E_StageClearConditionResult);
	// 		});
	// 	}
	// 	StageClearRuler->Init(this, M_StageRecorder, StageClearConditions, Test_ClearConditionChain ? 1 : 0, StageDefeatConditions);
	// 	StageClearRuler->Update();
	// }
}

void ATest_StageGameMode::BeginPlay()
{
	// HandleMatchIsWaitingToStart() 이 후 호출
	Super::BeginPlay();

	UCMAssetManager* const AssetManager = Cast<UCMAssetManager>(GEngine->AssetManager);
	if (IsValid(AssetManager))
	{
		const FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ATest_StageGameMode::OnAssetDataTypeLoaded, AssetManager->PlayerDataType);
		AssetManager->TryLoadPrimaryAssetWithTypeDelegateToCall(AssetManager->PlayerDataType, Delegate);
	}
}

void ATest_StageGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATest_StageGameMode::StartStage()
{
	Super::StartStage();

	UCMPlayerManager* PlayerManager = GetPlayerManager();
	for (auto KVP : PlayerManager->GetPlayerMap())
	{
		// UCMCharBattleComponent* CharBattleComponent = KVP.Value->GetBattleComponent();
		UCMStatComponent* StatComponent = KVP.Value->GetStatComponent();
		StatComponent->SetUltiGauge(StageData->DefaultUltiGauge);
		//StatComponent->AddCharStatInfo_ToAdd(StageData->CharStatInfoAdd);
	}
}

bool ATest_StageGameMode::InitTestData()
{
	StageData = LoadObject<UTest_StageData>(nullptr, *StageDataAssetRef);;
	if (false == IsValid(StageData) && IsValid(StageData_Default))
	{
		StageData = StageData_Default;
		return false;
	}

	UCMGameInstance* const GameInstance = Cast<UCMGameInstance>(GetGameInstance());
	if (GameInstance == nullptr)
	{
		return false;
	}

	UCMStageInfoManager* const StageInfoManager = GameInstance->GetStageInfoManager();
	if (StageInfoManager == nullptr)
	{
		return false;
	}

	UCMTacticInfoManager* const TacticInfoManager = GameInstance->GetTacticInfoManager();
	if (TacticInfoManager == nullptr)
	{
		return false;
	}

	UCMCharInfoManager* const CharInfoManager = GameInstance->GetCharInfoManager();
	if (CharInfoManager == nullptr)
	{
		return false;
	}

	UTBManager* const TBManager = GET_CM_MANAGER_TABLE();
	if (false == IsValid(TBManager))
	{
		return false;
	}

	UTBStage_Info_Record* const TBStage_Info_Record = TBManager->stage_info->Find(StageData->StageID);
	if (TBStage_Info_Record == nullptr)
	{
		return false;
	}

	StageInfoManager->SetSelectedStageID(StageData->StageID);

	int32 maxplayer = TBStage_Info_Record->PlayerSlotCount < StageData->PlayerList.Num() ? TBStage_Info_Record->PlayerSlotCount : StageData->PlayerList.Num();

	TacticInfoManager->InitSlot(maxplayer, StageData->StageID);

	for (int32 i = 0; i < maxplayer; ++i)
	{
		FGB_CharacterInfo CharInfo;

		CharInfo.opdata = static_cast<uint32>(EGB_OpTag::opInsert);
		CharInfo.CharID = StageData->PlayerList[i].ID;
		CharInfo.Class = StageData->PlayerList[i].Class;
		CharInfo.MemoryChip = 0;
		CharInfo.Lv = StageData->PlayerList[i].Level;
		CharInfo.LvExp = 0;
		CharInfo.FavorLv = 0;
		CharInfo.FavorLvExp = 0;
		CharInfo.CharLock = 0;
		CharInfo.EquipCostumeID = 0;
		for (int32 j = 0; j < 7; ++j)
		{
			CharInfo.SkillLv[j] = 1;
		}

		CharInfoManager->SetCharacterInfo(CharInfo, false);

		TacticInfoManager->SetSlotCharID(i, CharInfo.CharID);
	}

	// Deck
	FGB_StageDeckInfo2 StageDeckInfo;
	switch (TBStage_Info_Record->ModeSubType)
	{
	case ETB_GameMode_SubType::STORY_MAINSTORY:
		StageDeckInfo.DeckType = static_cast<int32>(ETB_Mode_Deck_Type::STORY_MAINSTORY_DECK);
		break;
	case ETB_GameMode_SubType::STORY_NARRATIVE:
		StageDeckInfo.DeckType = static_cast<int32>(ETB_Mode_Deck_Type::STORY_NARRATIVE_DECK);
		break;
	default:
		StageDeckInfo.DeckType = static_cast<int32>(ETB_Mode_Deck_Type::STORY_MAINSTORY_DECK);
		break;
	}

	TacticInfoManager->MakeDeck(StageDeckInfo);
	StageDeckInfo.OperatorCharID = TacticInfoManager->GetOperatorCharID();
	CharInfoManager->SetStageDeckInfo(StageDeckInfo);

	// Operator
	UCMTableInfoManager* const TableInfoManager = GET_CM_MANAGER_TABLEINFO();
	UTBSkill_Base_Record* SkillBaseRecord;
	if (TableInfoManager->Try_Get_TBSkill_Base_Record(SkillBaseRecord, StageData->Operator.ID, ETB_Skill_Slot::OPERATOR, 1))
	{
		FGB_CharacterInfo CharInfo;
		CharInfo.opdata = static_cast<uint32>(EGB_OpTag::opInsert);
		CharInfo.CharID = StageData->Operator.ID;
		CharInfo.Class = StageData->Operator.Class;
		CharInfo.MemoryChip = 0;
		CharInfo.Lv = StageData->Operator.Level;
		CharInfo.LvExp = 0;
		CharInfo.FavorLv = 0;
		CharInfo.FavorLvExp = 0;
		CharInfo.CharLock = 0;
		for (int32 j = 0; j < 7; ++j)
		{
			CharInfo.SkillLv[j] = 1;
		}

		CharInfoManager->SetCharacterInfo(CharInfo, false);

		TacticInfoManager->SetOpeatorCharID(StageData->Operator.ID);
		TacticInfoManager->SetOperatorSkillBase(SkillBaseRecord);
	}

	// Mission
	for (int32 i = 0; i < StageData->GameModeMissionIDs.Num(); i++)
	{
		int32 GameModeMissionID = StageData->GameModeMissionIDs[i];

		if (TBStage_Info_Record->GameModeMission.IsValidIndex(i))
		{
			TBStage_Info_Record->GameModeMission[i] = GameModeMissionID;
		}
		else
		{
			TBStage_Info_Record->GameModeMission.Add(GameModeMissionID);
		}
	}
	
	return true;
}

void ATest_StageGameMode::SetIsPlayerPawnAI(bool InIsAiOn)
{
	if (IsValid(PlayerManager))
	{
		PlayerManager->SetAllAuto(InIsAiOn);
		if (InIsAiOn)
		{
			PlayerManager->StartAllTree();
		}
		else
		{
			PlayerManager->StopAllTree();
		}
	}
}

void ATest_StageGameMode::SetIsMonsterPawnAI(bool InIsAiOn)
{
	TArray<ACMMonsterPawn*> MonsterList;
	LevelDataManager->GetMonsterList(MonsterList);

	for (int32 i = 0; i < MonsterList.Num(); i++)
	{
		ACMMonsterPawn* Monster = MonsterList[i];
		if (false == IsValid(Monster))
		{
			continue;
		}

		if (InIsAiOn)
		{
			Monster->GetMonsterAIController()->StartTree();
		}
		else
		{
			Monster->GetMonsterAIController()->StopTree();
		}
	}
}

void ATest_StageGameMode::OnAssetDataTypeLoaded(FPrimaryAssetType LoadedType)
{
	UCMAssetManager* const AssetManager = Cast<UCMAssetManager>(GEngine->AssetManager);
	if (IsValid(AssetManager))
	{
		AssetManager->OnAssetDataTypeLoaded(LoadedType);
		AssetManager->TryLoadPrimaryAssetsSoftObjectPathWithType(AssetManager->SkeletalMeshDataType);
	}
}