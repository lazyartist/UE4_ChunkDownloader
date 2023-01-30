// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IHttpRequest.h"

#include "CMChunkDownloader.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPatchCompleteDelegate, const bool, Succeeded);

UENUM()
enum EChunkDownloaderState
{
	None,
	
	PatchVersionDownload_Start,
	PatchVersionDownload_Succeed,
	PatchVersionDownload_Fail,
	
	ManifestUpdate_Start,
	ManifestUpdate_Succeed,
	ManifestUpdate_Fail,
	
	DownloadChunks_Start,
	DownloadChunks_Succeed,
	DownloadChunks_Fail,
	
	Mount_Start,
	Mount_Succeed,
	Mount_Fail,
	
	Complete,
};

USTRUCT()
struct FPPatchStats
{
	GENERATED_BODY()

	UPROPERTY()
	int32 FilesDownloaded = 0;

	UPROPERTY()
	int32 TotalFilesToDownload = 0;

	UPROPERTY()
	float DownloadPercent = 0.f;

	UPROPERTY()
	int32 MBDownloaded = 0;

	UPROPERTY()
	int32 TotalMBToDownload = 0;

	UPROPERTY()
	FText LastError;
};

USTRUCT()
struct FChunkDownloaderProgress
{
	GENERATED_BODY()

	int32 mBytesDownloaded = 0;
	int32 mTotalBytesToDownload = 0;
	// float mDownloadPercent = 0.f;
	int32 mChunkMounted = 0;
	int32 mTotalChunksToMount = 0;
	// float mMountPercent = 0.f;
};

DECLARE_DELEGATE_OneParam(FOnChunkDownloaderState_Updated, const EChunkDownloaderState);
DECLARE_DELEGATE_OneParam(FOnChunkDownloaderProgress_Update, const FChunkDownloaderProgress&);

UCLASS()
class UE4_PAKTEST_API ACMChunkDownloader : public AActor
{
	GENERATED_BODY()

public:
	ACMChunkDownloader();
	
	virtual void Tick(float DeltaTime) override;
	
	void InitPatchingSystem(const FString& InPatchPlatform, const TArray<int32>& InChunkDownloadList, const FString& InPatchVersionURL);
	void InitPatchingSystem(const FString& InPatchPlatform, const TArray<int32>& InChunkDownloadList);

	void InitChunkDownloader(const FString& InBuildID, const FString& InDeploymentName, const FString& InPlatformName);
	
protected:
	void OnPatchVersionDownloadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnManifestUpdateComplete(const bool bSuccess);
	void OnDownloadChunksComplete(const bool bSuccess);
	void OnLoadingModeComplete(const bool bSuccess);
	void OnMountComplete(const bool bSuccess);

public:
	UFUNCTION()
	void GetLoadingProgress(int32& BytesDownloaded, int32& TotalBytesToDownload, float& DownloadPercent, int32& ChunkMounted, int32& TotalChunksToMount, float& MountPercent) const;

	UFUNCTION()
	bool DownloadChunks();

	UFUNCTION()
	const int32 GetChunkDownloaderListCount() const { return mChunkIDsToDownload.Num(); }

	UFUNCTION()
	FPPatchStats GetPatchStatus();

	UFUNCTION()
	void GetDiskTotalAndFreeSpace(const FString& InPath, uint64& TotalSpace, uint64& FreeSpace);

	UPROPERTY()
	FPatchCompleteDelegate OnPatchComplete;

	FOnChunkDownloaderState_Updated OnChunkDownloaderState_Update;
	
	FOnChunkDownloaderProgress_Update OnChunkDownloaderProgress_Update;

	EChunkDownloaderState GetChunkDownloaderStatus() { return mEChunkDownloaderState; };

protected:
	FString mPlatformName;

	UPROPERTY()
	FString mPatchVersionURL;

	bool bIsDownloadManifestUpToDate;

	UPROPERTY()
	TArray<int32> mChunkIDsToDownload;

	EChunkDownloaderState mEChunkDownloaderState = EChunkDownloaderState::None;

private:
	void SetChunkDownloaderStatus(const EChunkDownloaderState InEChunkDownloaderState);
	void UpdateProgress();
};
