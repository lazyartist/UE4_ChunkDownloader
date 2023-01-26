// Fill out your copyright notice in the Description page of Project Settings.

#include "CMChunkDownloader.h"

#include "ChunkDownloader.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Math/UnitConversion.h"
#include "CMCommon.h"


// Sets default values
// UCMChunkDownloader::UCMChunkDownloader()
// {
// 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
// 	// PrimaryActorTick.bCanEverTick = true;
// }

// // Called when the game starts or when spawned
// void UCMChunkDownloader::BeginPlay()
// {
// 	Super::BeginPlay();
// 	
// }
//
// // Called every frame
// void UCMChunkDownloader::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);
// }
//


void UCMChunkDownloader::OnPatchingSystemVersionResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		FString ContentBuildId = Response->GetContentAsString();

		CM_LOG(Log, "ContentBuildId(%s)", *ContentBuildId);
		InitChunkDownloader(ContentBuildId);
	}
}

/////////////////// ChunkDownloader /////////////////// 
void UCMChunkDownloader::InitPatchingSystem(const FString& InPatchPlatform, const FString& InPatchVersionURL, const TArray<int32>& InChunkDownloadList)
{
	mPatchPlatform = InPatchPlatform;
	mPatchVersionURL = InPatchVersionURL;
	mChunkDownloadList = InChunkDownloadList;

	FHttpModule& Http = FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http.CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UCMChunkDownloader::OnPatchingSystemVersionResponse);

	Request->SetURL(InPatchVersionURL);
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->ProcessRequest();
}

void UCMChunkDownloader::InitChunkDownloader(const FString& BuildID)
{
	const FString DeploymentName = "Dev"; // PatchingDemoLive
	const FString PlatformName = "Android";
	// const FString PlatformName = "Windows";

	CM_LOG(Log, "DeploymentName(%s)", *DeploymentName);
	CM_LOG(Log, "PlatformName(%s)", *PlatformName);

	TSharedRef<FChunkDownloader> Downloader = FChunkDownloader::GetOrCreate();
	Downloader->Initialize(PlatformName, 8);
	Downloader->LoadCachedBuild(DeploymentName);
	TFunction<void(bool bSuccess)> UpdateCompleteCallback = [&](bool bSuccess)
	{
		OnManifestUpdateComplete(bSuccess);
	};
	Downloader->UpdateBuild(DeploymentName, BuildID, UpdateCompleteCallback);
}

void UCMChunkDownloader::OnManifestUpdateComplete(const bool bSuccess)
{
	CM_LOG(Log, "bSuccess(%d)", bSuccess);
	
	bIsDownloadManifestUpToDate = bSuccess;
	PatchGame();
}

void UCMChunkDownloader::OnDownloadComplete(const bool bSuccess)
{
	CM_LOG(Log, "bSuccess(%d)", bSuccess);
	
	if (bSuccess)
	{
		TSharedRef<FChunkDownloader> Downloader = FChunkDownloader::GetChecked();
		FJsonSerializableArrayInt DownloadedChunks;
		for (int32 ChunkID : mChunkDownloadList)
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

void UCMChunkDownloader::OnLoadingModeComplete(const bool bSuccess)
{
	CM_LOG(Log, "");
	OnDownloadComplete(bSuccess);
}

void UCMChunkDownloader::OnMountComplete(const bool bSuccess)
{
	CM_LOG(Log, "");
	OnPatchComplete.Broadcast(bSuccess);
}

void UCMChunkDownloader::GetLoadingProgress(int32& BytesDownloaded, int32& TotalBytesToDownload, float& DownloadPercent, int32& ChunkMounted, int32& TotalChunksToMount, float& MountPercent) const
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

bool UCMChunkDownloader::PatchGame()
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
		TSharedRef<FChunkDownloader> Downloader = FChunkDownloader::GetChecked();
		for (int32 ChunkID : mChunkDownloadList)
		{
			FChunkDownloader::EChunkStatus eChunkStatus = Downloader->GetChunkStatus(ChunkID);
			CM_LOG(Log, "ChunkID(%d), eChunkStatus(%s)", ChunkID, *EChunkStatusToString(eChunkStatus));
			// int32 ChunkStatus = static_cast<int32>(Downloader->GetChunkStatus(ChunkID));
		}

// #if 1
		CM_LOG(Log, "Downloader->DownloadChunks");
		TFunction<void(const bool bSuccess)> DownloadCompleteCallback = [&](const bool bSuccess) { OnDownloadComplete(bSuccess); };
		Downloader->DownloadChunks(mChunkDownloadList, DownloadCompleteCallback, 1);

		TFunction<void(const bool bSuccess)> LoadingModeCompleteCallback = [&](const bool bSuccess) { OnLoadingModeComplete(bSuccess); };
		Downloader->BeginLoadingMode(LoadingModeCompleteCallback);
// #else
// 		TFunction<void(bool)> MountCompleteCallback = [&](const bool bSuccess) { bSuccess ? OnPatchComplete.Broadcast(true) : OnPatchComplete.Broadcast(false); };
// 		Downloader->MountChunks(ChunkDownloadList, MountCompleteCallback);
// #endif
		return true;
	}
	return false;
}

FPPatchStats UCMChunkDownloader::GetPatchStatus()
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

void UCMChunkDownloader::GetDiskTotalAndFreeSpace(const FString& InPath, uint64& TotalSpace, uint64& FreeSpace)
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

/////////////////// ChunkDownloader /////////////////// end
