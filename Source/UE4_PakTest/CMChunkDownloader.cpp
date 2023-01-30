﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "CMChunkDownloader.h"

#include "ChunkDownloader.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Math/UnitConversion.h"
#include "CMCommon.h"

ACMChunkDownloader::ACMChunkDownloader()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACMChunkDownloader::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateProgress();
}

void ACMChunkDownloader::OnPatchVersionDownloadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		SetChunkDownloaderStatus(EChunkDownloaderState::PatchVersionDownload_Succeed);
		
		const FString ResponseContent = Response->GetContentAsString(); // BuildId = v1.0.1, v1.0.2, ...
		FString ResponseContentAsString = Response->GetContentAsString();
		TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(ResponseContentAsString);
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		FJsonSerializer::Deserialize(Reader, JsonObject);

		const FString BuildId = JsonObject->GetStringField(TEXT("BuildId"));
		const FString DeploymentName = JsonObject->GetStringField(TEXT("DeploymentName"));
		const FString PlatformName = JsonObject->GetStringField(TEXT("PlatformName"));
		
		// const FString BuildId = Response->GetContentAsString(); // BuildId = v1.0.1, v1.0.2, ...
		// const FString DeploymentName = "Dev"; // DefaultGame.ini 파일의 [/Script/Plugins.ChunkDownloader Dev] 항목을 찾기 위한 이름, Dev 부분이 Dev, QA, Service 등으로 설정되어있다. 
		// const FString PlatformName = mPlatformName;
		
		CM_LOG(Log, "BuildId(%s), DeploymentName(%s), PlatformName(%s)", *BuildId, *DeploymentName, *PlatformName); 

		InitChunkDownloader(BuildId, DeploymentName, PlatformName);
	}
	else
	{
		SetChunkDownloaderStatus(EChunkDownloaderState::PatchVersionDownload_Fail);
	}
}

void ACMChunkDownloader::InitPatchingSystem(const FString& InPatchPlatform, const TArray<int32>& InChunkDownloadList, const FString& InPatchVersionURL)
{
	mPlatformName = InPatchPlatform;
	mChunkIDsToDownload = InChunkDownloadList;
	mPatchVersionURL = InPatchVersionURL;

	FHttpModule& Http = FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http.CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ACMChunkDownloader::OnPatchVersionDownloadComplete);

	Request->SetURL(InPatchVersionURL);
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->ProcessRequest();
	
	SetChunkDownloaderStatus(EChunkDownloaderState::PatchVersionDownload_Start);
}

void ACMChunkDownloader::InitPatchingSystem(const FString& InPatchPlatform, const TArray<int32>& InChunkDownloadList)
{
	mPlatformName = InPatchPlatform;
	mChunkIDsToDownload = InChunkDownloadList;
	// mPatchVersionURL = InPatchVersionURL;

	// FHttpModule& Http = FHttpModule::Get();
	// TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http.CreateRequest();
	// Request->OnProcessRequestComplete().BindUObject(this, &ACMChunkDownloader::OnPatchVersionDownloadComplete);
	//
	// Request->SetURL(InPatchVersionURL);
	// Request->SetVerb("GET");
	// Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	// Request->SetHeader("Content-Type", TEXT("application/json"));
	// Request->ProcessRequest();
	//
	// SetChunkDownloaderStatus(EChunkDownloaderState::PatchVersionDownload_Start);
}

void ACMChunkDownloader::InitChunkDownloader(const FString& InBuildID, const FString& InDeploymentName, const FString& InPlatformName)
{
	SetChunkDownloaderStatus(EChunkDownloaderState::ManifestUpdate_Start);
	
	TSharedRef<FChunkDownloader> ChunkDownloader = FChunkDownloader::GetOrCreate();

	ChunkDownloader->Finalize(); // 이미 Mount된 청크를 Unmount한다.
	
	ChunkDownloader->Initialize(InPlatformName, 8);
	ChunkDownloader->LoadCachedBuild(InDeploymentName);

	TFunction<void(bool bSuccess)> UpdateCompleteCallback = [&](bool bSuccess)
	{
		OnManifestUpdateComplete(bSuccess);
	};
	ChunkDownloader->UpdateBuild(InDeploymentName, InBuildID, UpdateCompleteCallback);
}

void ACMChunkDownloader::OnManifestUpdateComplete(const bool bSuccess)
{
	bIsDownloadManifestUpToDate = bSuccess;

	if(bSuccess)
	{
		SetChunkDownloaderStatus(EChunkDownloaderState::ManifestUpdate_Succeed);
		
		DownloadChunks();
	}
	else
	{
		SetChunkDownloaderStatus(EChunkDownloaderState::ManifestUpdate_Fail);
	}
}

void ACMChunkDownloader::OnDownloadChunksComplete(const bool bSuccess)
{
	if (bSuccess)
	{
		SetChunkDownloaderStatus(EChunkDownloaderState::DownloadChunks_Succeed);
		SetChunkDownloaderStatus(EChunkDownloaderState::Mount_Start);
		
		TSharedRef<FChunkDownloader> Downloader = FChunkDownloader::GetChecked();
		FJsonSerializableArrayInt ChunkIDsToMount;
		for (int32 ChunkID : mChunkIDsToDownload)
		{
			ChunkIDsToMount.Add(ChunkID);
		}
		TFunction<void(const bool bSuccess)> MountCompleteCallback = [&](const bool bSuccess) { OnMountComplete(bSuccess); };
		Downloader->MountChunks(ChunkIDsToMount, MountCompleteCallback);
		OnPatchComplete.Broadcast(true);
	}
	else
	{
		SetChunkDownloaderStatus(EChunkDownloaderState::DownloadChunks_Fail);
		OnPatchComplete.Broadcast(false);
	}
}

void ACMChunkDownloader::OnLoadingModeComplete(const bool bSuccess)
{
	CM_LOG(Log, "bSuccess(%d)", bSuccess);
	// OnDownloadChunksComplete(bSuccess);
}

void ACMChunkDownloader::OnMountComplete(const bool bSuccess)
{
	CM_LOG(Log, "bSuccess(%d)", bSuccess);
	OnPatchComplete.Broadcast(bSuccess);
	if(bSuccess)
	{
		SetChunkDownloaderStatus(EChunkDownloaderState::Mount_Succeed);
	}
	else
	{
		SetChunkDownloaderStatus(EChunkDownloaderState::Mount_Fail);
	}

	SetChunkDownloaderStatus(EChunkDownloaderState::Complete);
}

void ACMChunkDownloader::GetLoadingProgress(int32& BytesDownloaded, int32& TotalBytesToDownload, float& DownloadPercent, int32& ChunkMounted, int32& TotalChunksToMount, float& MountPercent) const
{
	TSharedRef<FChunkDownloader> ChunkDownloader = FChunkDownloader::GetChecked();
	FChunkDownloader::FStats LoadingStats = ChunkDownloader->GetLoadingStats();

	BytesDownloaded = LoadingStats.BytesDownloaded;
	TotalBytesToDownload = LoadingStats.TotalBytesToDownload;

	ChunkMounted = LoadingStats.ChunksMounted;
	TotalChunksToMount = LoadingStats.TotalChunksToMount;

	DownloadPercent = ((float)BytesDownloaded / (float)TotalBytesToDownload) * 100.f;
	MountPercent = ((float)ChunkMounted / (float)TotalChunksToMount) * 100.f;
}

bool ACMChunkDownloader::DownloadChunks()
{
	auto EChunkStatusToString = [](FChunkDownloader::EChunkStatus In_EChunkStatus) -> FString
	{
		switch (In_EChunkStatus) {
			case FChunkDownloader::EChunkStatus::Mounted:
				return "Mounted";
			case FChunkDownloader::EChunkStatus::Cached: 
				return "Cached";
			case FChunkDownloader::EChunkStatus::Downloading: 
				return "Downloading";
			case FChunkDownloader::EChunkStatus::Partial: 
				return "Partial";
			case FChunkDownloader::EChunkStatus::Remote: 
				return "Remote";
			case FChunkDownloader::EChunkStatus::Unknown: 
				return "Unknown";
			default: ;
		}
	
		return "";
	};
	
	if (bIsDownloadManifestUpToDate)
	{
		TSharedRef<FChunkDownloader> ChunkDownloader = FChunkDownloader::GetChecked();
		for (int32 ChunkID : mChunkIDsToDownload)
		{
			FChunkDownloader::EChunkStatus eChunkStatus = ChunkDownloader->GetChunkStatus(ChunkID);
			CM_LOG(Log, "ChunkID(%d), eChunkStatus(%s)", ChunkID, *EChunkStatusToString(eChunkStatus));
		}

// #if 1
		SetChunkDownloaderStatus(EChunkDownloaderState::DownloadChunks_Start);
		
		TFunction<void(const bool bSuccess)> DownloadCompleteCallback = [&](const bool bSuccess) { OnDownloadChunksComplete(bSuccess); };
		ChunkDownloader->DownloadChunks(mChunkIDsToDownload, DownloadCompleteCallback, 1);

		TFunction<void(const bool bSuccess)> LoadingModeCompleteCallback = [&](const bool bSuccess) { OnLoadingModeComplete(bSuccess); };
		ChunkDownloader->BeginLoadingMode(LoadingModeCompleteCallback);

// #else
// 		TFunction<void(bool)> MountCompleteCallback = [&](const bool bSuccess) { bSuccess ? OnPatchComplete.Broadcast(true) : OnPatchComplete.Broadcast(false); };
// 		Downloader->MountChunks(ChunkDownloadList, MountCompleteCallback);
// #endif
		return true;
	}
	return false;
}

FPPatchStats ACMChunkDownloader::GetPatchStatus()
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

void ACMChunkDownloader::GetDiskTotalAndFreeSpace(const FString& InPath, uint64& TotalSpace, uint64& FreeSpace)
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

void ACMChunkDownloader::SetChunkDownloaderStatus(const EChunkDownloaderState InEChunkDownloaderState)
{
	CM_LOG(Log, "InEChunkDownloaderStatus(%s)", *CMUtils::EnumToFString(TEXT("EChunkDownloaderState"), InEChunkDownloaderState));
	
	mEChunkDownloaderState = InEChunkDownloaderState;
	
	OnChunkDownloaderState_Update.ExecuteIfBound(InEChunkDownloaderState);

	UpdateProgress();
}

void ACMChunkDownloader::UpdateProgress()
{
	switch (mEChunkDownloaderState)
	{
	case None:
	case PatchVersionDownload_Start:
	case PatchVersionDownload_Succeed:
	case PatchVersionDownload_Fail:
	case ManifestUpdate_Start:
	case ManifestUpdate_Succeed:
	case ManifestUpdate_Fail:
	case Complete:
		{
		return;
		}
	case DownloadChunks_Start:
	case DownloadChunks_Succeed:
	case DownloadChunks_Fail:
	case Mount_Start:
	case Mount_Succeed:
	case Mount_Fail:
		break;
	default: ;
	}
	
	FChunkDownloaderProgress FChunkDownloaderProgress;

	TSharedRef<FChunkDownloader> ChunkDownloader = FChunkDownloader::GetChecked();
	FChunkDownloader::FStats LoadingStats = ChunkDownloader->GetLoadingStats();

	FChunkDownloaderProgress.mBytesDownloaded = LoadingStats.BytesDownloaded;
	FChunkDownloaderProgress.mTotalBytesToDownload = LoadingStats.TotalBytesToDownload;

	FChunkDownloaderProgress.mChunkMounted = LoadingStats.ChunksMounted;
	FChunkDownloaderProgress.mTotalChunksToMount = LoadingStats.TotalChunksToMount;

	OnChunkDownloaderProgress_Update.ExecuteIfBound(FChunkDownloaderProgress);
}