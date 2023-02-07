// Fill out your copyright notice in the Description page of Project Settings.


#include "UIMain_Level0.h"

#include "CMCommon.h"
#include "CMCommon.h"
#include "CMGameInstance.h"

void UUIMain_Level0::NativePreConstruct()
{
	Super::NativePreConstruct();

	// if (IsValid(DownloadButton_BP))
	// {
	// 	DownloadButton_BP->OnClicked.AddDynamic(this, &UUIMain_Level0::DownloadButton_OnClicked);
	// }

	// if(IsValid(Download0Button_BP))
	// {
	// 	Download0Button_BP->OnClicked.AddDynamic(this, &UUIMain_Level0::Download0Button_OnClicked);
	// }
	//
	// if(IsValid(Download1Button_BP))
	// {
	// 	Download1Button_BP->OnClicked.AddDynamic(this, &UUIMain_Level0::Download1Button_OnClicked);
	// }
	//
	// if(IsValid(Download2Button_BP))
	// {
	// 	Download2Button_BP->OnClicked.AddDynamic(this, &UUIMain_Level0::Download2Button_OnClicked);
	// }
	//
	// if(IsValid(OpenLevel0Button_BP))
	// {
	// 	OpenLevel0Button_BP->OnClicked.AddDynamic(this, &UUIMain_Level0::OpenLevel0Button_OnClicked);
	// }
	//
	// if(IsValid(OpenLevel1Button_BP))
	// {
	// 	OpenLevel1Button_BP->OnClicked.AddDynamic(this, &UUIMain_Level0::OpenLevel1Button_OnClicked);
	// }
	//
	// if(IsValid(OpenLevel2Button_BP))
	// {
	// 	OpenLevel2Button_BP->OnClicked.AddDynamic(this, &UUIMain_Level0::OpenLevel2Button_OnClicked);
	// }
}

void UUIMain_Level0::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UUIMain_Level0::DownloadButton_OnClicked()
{
	DownloadChunk();
}

void UUIMain_Level0::DownloadChunk()
{
	UCMGameInstance* GameInstance = Cast<UCMGameInstance>(GetWorld()->GetGameInstance());
	if (false == IsValid(GameInstance))
	{
		CM_LOG(Error, "UPGGameInstance is Wrong!!!");
		return;
	}
	
	if (false == IsValid(mChunkDownloader))
	{
		mChunkDownloader = NewObject<UCMChunkDownloader>(GetWorld());
		// mChunkDownloader = NewObject<ACMChunkDownloader>(this);
	}

	if (IsValid(mChunkDownloader))
	{
		if (false == mChunkDownloader->OnChunkDownloaderState_Update.IsBound())
		{
			mChunkDownloader->OnChunkDownloaderState_Update.BindWeakLambda(this, [this](const EChunkDownloaderState InEChunkDownloaderStatus)
			{
				if (false == ChunkDownloaderStateLog.IsEmpty())
				{
					ChunkDownloaderStateLog.Append("\n");
				}
				ChunkDownloaderStateLog.Append(CMUtils::EnumToFString(TEXT("EChunkDownloaderState"), InEChunkDownloaderStatus));
				ChunkDownloaderStateText_BP->SetText(FText::FromString(ChunkDownloaderStateLog));
			});
		}

		if (false == mChunkDownloader->OnChunkDownloaderProgress_Update.IsBound())
		{
			mChunkDownloader->OnChunkDownloaderProgress_Update.BindWeakLambda(this, [this](const FChunkDownloaderProgress& InChunkDownloaderProgress)
			{
				UpdateChunkDownloaderProgress(InChunkDownloaderProgress);
			});
		}

		ChunkDownloaderStateLog.Empty();

		// mChunkDownloader->InitPatchingSystem("Android", GameInstance->ChunkDownloadList, TEXT("http://10.10.11.159:18080/PatchVersion.json"));
		mChunkDownloader->InitPatchingSystem(GameInstance->PatchVersionURL);
	}
}

void UUIMain_Level0::DownloadChunk_With_PatchVersionURL(const FString& InBuildID)
{
	UCMGameInstance* GameInstance = Cast<UCMGameInstance>(GetWorld()->GetGameInstance());
	if (false == IsValid(GameInstance))
	{
		CM_LOG(Error, "UPGGameInstance is Wrong!!!");
		return;
	}
	
	if (false == IsValid(mChunkDownloader))
	{
		mChunkDownloader = NewObject<UCMChunkDownloader>(GetWorld());
		// mChunkDownloader = NewObject<ACMChunkDownloader>(this);
	}

	if (IsValid(mChunkDownloader))
	{
		if (false == mChunkDownloader->OnChunkDownloaderState_Update.IsBound())
		{
			mChunkDownloader->OnChunkDownloaderState_Update.BindWeakLambda(this, [this](const EChunkDownloaderState InEChunkDownloaderStatus)
			{
				if (false == ChunkDownloaderStateLog.IsEmpty())
				{
					ChunkDownloaderStateLog.Append("\n");
				}
				ChunkDownloaderStateLog.Append(CMUtils::EnumToFString(TEXT("EChunkDownloaderState"), InEChunkDownloaderStatus));
				ChunkDownloaderStateText_BP->SetText(FText::FromString(ChunkDownloaderStateLog));
			});
		}

		if (false == mChunkDownloader->OnChunkDownloaderProgress_Update.IsBound())
		{
			mChunkDownloader->OnChunkDownloaderProgress_Update.BindWeakLambda(this, [this](const FChunkDownloaderProgress& InChunkDownloaderProgress)
			{
				UpdateChunkDownloaderProgress(InChunkDownloaderProgress);
			});
		}

		ChunkDownloaderStateLog.Empty();

		mChunkDownloader->InitPatchingSystem(GameInstance->PatchVersionURL);
		mChunkDownloader->InitChunkDownloader(InBuildID, "Dev", "Android");
	}
}

void UUIMain_Level0::UpdateChunkDownloaderProgress(const FChunkDownloaderProgress& InChunkDownloaderProgress)
{
	{
		if (IsValid(mChunkDownloader))
		{
			FString ChunkDownloaderProgress;

			int32 BytesDownloaded = InChunkDownloaderProgress.mBytesDownloaded;
			int32 TotalBytesToDownload = InChunkDownloaderProgress.mTotalBytesToDownload;
			int32 ChunkMounted = InChunkDownloaderProgress.mChunkMounted;
			int32 TotalChunksToMount = InChunkDownloaderProgress.mTotalChunksToMount;
			
			int32 DownloadPercent = ((float)BytesDownloaded / (float)TotalBytesToDownload) * 100.f;
			float MountPercent = ((float)ChunkMounted / (float)TotalChunksToMount) * 100.f;
			
			// mChunkDownloader->GetLoadingProgress(BytesDownloaded, TotalBytesToDownload, DownloadPercent, ChunkMounted, TotalChunksToMount, MountPercent);

			FString CurProgress, MaxProgress, OutputStr;
			CurProgress = FString::SanitizeFloat(DownloadPercent);
			OutputStr = CurProgress;

			ChunkDownloaderProgress.Append(FString::Printf(TEXT("BytesDownloaded(%d), TotalBytesToDownload(%d), DownloadPercent(%f), ChunkMounted(%d), TotalChunksToMount(%d), MountPercent(%f)"), BytesDownloaded, TotalBytesToDownload, DownloadPercent, ChunkMounted, TotalChunksToMount, MountPercent));
			ChunkDownloaderProgress.Append("\n");
			ChunkDownloaderProgress.Append(FString::Printf(TEXT("CurProgress(%s), MaxProgress(%s), OutputStr(%s)"), *CurProgress, *MaxProgress, *OutputStr));

			ChunkDownloaderProgressText_BP->SetText(FText::FromString(ChunkDownloaderProgress));
			CM_LOG(Log, "%s", *ChunkDownloaderProgress);

			// if (GEngine)
			// {
			// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, OutputStr);
			// }
		}
	}
}

void UUIMain_Level0::InternalDownloadChunkData()
{
	if (false == IsValid(mChunkDownloader))
	{
		mChunkDownloader = NewObject<UCMChunkDownloader>(GetWorld());
		// mChunkDownloader = NewObject<ACMChunkDownloader>(this);
	}

	if (IsValid(mChunkDownloader))
	{
		if (false == mChunkDownloader->OnChunkDownloaderState_Update.IsBound())
		{
			mChunkDownloader->OnChunkDownloaderState_Update.BindWeakLambda(this, [this](const EChunkDownloaderState InEChunkDownloaderStatus)
			{
				OnChunkDownloaderState_Update(InEChunkDownloaderStatus);
			});
		}

		if (false == mChunkDownloader->OnChunkDownloaderProgress_Update.IsBound())
		{
			mChunkDownloader->OnChunkDownloaderProgress_Update.BindWeakLambda(this, [this](const FChunkDownloaderProgress& InChunkDownloaderProgress)
			{
				OnChunkDownloaderProgress_Update(InChunkDownloaderProgress);
			});
		}

		UCMGameInstance* GameInstance = Cast<UCMGameInstance>(GetWorld()->GetGameInstance());
		if (false == IsValid(GameInstance))
		{
			CM_LOG(Error, "UPGGameInstance is Wrong!!!");
			return;
		}

		mChunkDownloader->InitPatchingSystem(GameInstance->PatchVersionURL);
		
		// ChunkDownloaderStateLog.Empty();
		
		// FServiceInfo ServiceInfo;
		// if(TryGetCurrentServiceInfo(ServiceInfo))
		// {
		// 	ClearProgressBar();
		// 	
		// 	mChunkDownloader->InitPatchingSystem(ServiceInfo.PatchVersionURL);
		// }
	}
}

void UUIMain_Level0::OnChunkDownloaderState_Update(const EChunkDownloaderState InEChunkDownloaderState)
{
	mEChunkDownloaderState = InEChunkDownloaderState;
	
	CM_LOG(Log, "%s", *CMUtils::EnumToFString(TEXT("EChunkDownloaderState"), mEChunkDownloaderState));
	
	// if (false == ChunkDownloaderStateLog.IsEmpty())
	// {
	// 	ChunkDownloaderStateLog.Append("\n");
	// }
	// ChunkDownloaderStateLog.Append(CMUtils::EnumToFString(TEXT("EChunkDownloaderState"), mEChunkDownloaderState));
	// ChunkDownloaderStateText_BP->SetText(FText::FromString(ChunkDownloaderStateLog));

	switch (mEChunkDownloaderState)
	{
	case None: break;
	case PatchVersionDownload_Start:
	case PatchVersionDownload_Succeed:
	case PatchVersionParsing_Succeed:
	case ManifestUpdate_Start:
	case ManifestUpdate_Succeed:
	case DownloadChunks_Start:
	case DownloadChunks_Succeed:
	case Mount_Start:
	case Mount_Succeed:
		{
			//
		}
		break;
	case PatchVersionDownload_Fail:
	case PatchVersionParsing_Fail:
	case ManifestUpdate_Fail:
	case DownloadChunks_Fail:
	case Mount_Fail:
		{
			// SetAppInitStepType(ECM_AppInitStepType::Download_ChunkData_Fail);
			// IsChunkDataComplete = false;
			//
			// DECLARE_GAMEINSTANCE()
			// DECLARE_UI()
			//
			// FServiceInfo ServiceInfo;
			// if (false == TryGetCurrentServiceInfo(ServiceInfo))
			// {
			// 	return;
			// }
			//
			// UISystemManager->Open_YesNoPopup(*ServiceInfo.ServiceName, FString::Printf(TEXT("게임 리소스 다운로드에 실패했습니다.\n다시 시도하시겠습니까?\n(%s)"), *CMUtils::EnumToFString(TEXT("EChunkDownloaderState"), mEChunkDownloaderState)),
			// // Yes
			// [this](UCMPanelBase* InPanelBase)
			// {
			// 	DownloadServiceInfoFile(true);
			// },
			// // No
			// [this](UCMPanelBase* InPanelBase)
			// {
			// 	//
			// }, false);
		}
		break;
	case Complete:
		{
			// SetAppInitStepType(ECM_AppInitStepType::Download_ChunkData_Succeed);
			// IsChunkDataComplete = true;
		}
		break;
	default: ;
	}
	
	// UpdateReadyToLogin();
}

void UUIMain_Level0::OnChunkDownloaderProgress_Update(const FChunkDownloaderProgress& InChunkDownloaderProgress)
{
	if (IsValid(mChunkDownloader))
	{
		FString ChunkDownloaderProgress;

		uint64 BytesDownloaded = InChunkDownloaderProgress.mBytesDownloaded;
		uint64 TotalBytesToDownload = InChunkDownloaderProgress.mTotalBytesToDownload;
		int ChunkMounted = InChunkDownloaderProgress.mChunkMounted;
		int TotalChunksToMount = InChunkDownloaderProgress.mTotalChunksToMount;

		FString ProgressText;
		float CurruntProgress = 0.f;
		if (0.f != TotalBytesToDownload)
		{
			CurruntProgress = ((float)BytesDownloaded / (float)TotalBytesToDownload);

			// FServiceInfo ServiceInfo;
			// if (false == TryGetCurrentServiceInfo(ServiceInfo))
			// {
			// 	return;
			// }
			
			ProgressText = FString::Printf(TEXT("Resource Download(%s) %.1f%%(%.2f/%.2fMb)"), *mChunkDownloader->GetBuildID(), CurruntProgress * 100.f, ((float)BytesDownloaded/(float)(1024*1024)), ((float)TotalBytesToDownload/(float)(1024*1024)));
		}
			
		float MountPercent = 0.f;
		if (0.f != TotalChunksToMount)
		{
			MountPercent = ((float)ChunkMounted / (float)TotalChunksToMount) * 100.f;
		}
			
		FString CurProgress, MaxProgress, OutputStr;
		CurProgress = FString::SanitizeFloat(CurruntProgress);
		OutputStr = CurProgress;

		ChunkDownloaderProgress.Append(FString::Printf(TEXT("BytesDownloaded(%lld), TotalBytesToDownload(%lld), DownloadPercent(%f), ChunkMounted(%d), TotalChunksToMount(%d), MountPercent(%f)"), BytesDownloaded, TotalBytesToDownload, CurruntProgress, ChunkMounted, TotalChunksToMount, MountPercent));
		ChunkDownloaderProgress.Append("\n");
		ChunkDownloaderProgress.Append(FString::Printf(TEXT("CurProgress(%s), MaxProgress(%s), OutputStr(%s)"), *CurProgress, *MaxProgress, *OutputStr));

		// ChunkDownloaderProgressText_BP->SetText(FText::FromString(ChunkDownloaderProgress));
		// CM_LOG(Log, "%s", *ChunkDownloaderProgress);

		// mTargetProgress = CurruntProgress;
		
		// SetProgressText(ProgressText);
	}
}

// void UUIMain_Level0::Download0Button_OnClicked()
// {
// 	DownloadChunk_With_PatchVersionURL();
// }
//
// void UUIMain_Level0::OpenLevel1Button_OnClicked()
// {
// 	OpenLevel(mLevelNames[1]);
// }
//
// void UUIMain_Level0::OpenLevel2Button_OnClicked()
// {
// 	OpenLevel(mLevelNames[2]);
// }

// void UUIMain_Level0::Download0Button_OnClicked()
// {
// 	UE_LOG(LogTemp, Log, TEXT("Clicked"));
//
// 	// UMobilePatchingLibrary::RequestContent
// 	// RemoteManifestURL: http://localhost/files/ManifestDir/UE4_PakTest_pakchunk1release1.manifest
// 	// CloudURL: D:\www\files\CloudDir
// 	// InstallDirectory: InstallDirectory
//
// 	// adb logcat -s UE4 -s debug -s DEBUG
// }

// void UUIMain_Level0::OpenLevel0Button_OnClicked()
// {
// 	OpenLevel(mLevelNames[0]);
// 	// OpenLevel(TEXT("StartAssets/Level0"));
// }
//
// void UUIMain_Level0::OpenLevel1Button_OnClicked()
// {
// 	OpenLevel(mLevelNames[1]);
// }
//
// void UUIMain_Level0::OpenLevel2Button_OnClicked()
// {
// 	OpenLevel(mLevelNames[2]);
// }

// void UUIMain_Level0::OpenLevel(const FString In_LevelName)
// {
// 	UE_LOG(LogTemp, Log, TEXT("OpenLevel %s"), *In_LevelName);
// 	
// 	UGameplayStatics::OpenLevel(GetWorld(), *In_LevelName, true);
// }

// void UUIMain_Level0::RequestContent_OnSucceeded(UObject* MobilePendingContent)
// {
// 	UE_LOG(LogTemp, Log, TEXT("RequestContent_OnSucceeded"));
// }
//
// void UUIMain_Level0::RequestContent_OnFailed(FText ErrorText, int32 ErrorCode)
// {
// 	UE_LOG(LogTemp, Log, TEXT("RequestContent_OnFailed"));
// }
