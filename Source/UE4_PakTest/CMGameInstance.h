// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IHttpRequest.h"
#include "CMGameInstance.generated.h"

// /////////////////// ChunkDownloader ///////////////////
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPatchCompleteDelegate, const bool, Succeeded);
//
// USTRUCT()
// struct FPPatchStats
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY()
// 	int32 FilesDownloaded = 0;
//
// 	UPROPERTY()
// 	int32 TotalFilesToDownload = 0;
//
// 	UPROPERTY()
// 	float DownloadPercent = 0.f;
//
// 	UPROPERTY()
// 	int32 MBDownloaded = 0;
//
// 	UPROPERTY()
// 	int32 TotalMBToDownload = 0;
//
// 	UPROPERTY()
// 	FText LastError;
// };
// /////////////////// ChunkDownloader /////////////////// end


UCLASS()
class UE4_PAKTEST_API UCMGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	UCMGameInstance();

// protected:
// 	// Called when the game starts or when spawned
// 	virtual void BeginPlay() override;
//
// public:
// 	// Called every frame
// 	virtual void Tick(float DeltaTime) override;


	
// 	/////////////////// ChunkDownloader ///////////////////
// public:
// 	void InitPatchingSystem(const FString& InPatchPlatform);
// 	
// protected:
// 	void OnPatchingSystemVersionResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
// 	void InitChunkDownloader(const FString& BuildID);
// 	void OnManifestUpdateComplete(const bool bSuccess);
// 	void OnDownloadComplete(const bool bSuccess);
// 	void OnLoadingModeComplete(const bool bSuccess);
// 	void OnMountComplete(const bool bSuccess);
//
// public:
// 	UFUNCTION()
// 	void GetLoadingProgress(int32& BytesDownloaded, int32& TotalBytesToDownload, float& DownloadPercent, int32& ChunkMounted, int32& TotalChunksToMount, float& MountPercent) const;
//
// 	UFUNCTION()
// 	bool PatchGame();
//
// 	UFUNCTION()
// 	const int32 GetChunkDownloaderListCount() const { return ChunkDownloadList.Num(); }
//
// 	UFUNCTION()
// 	FPPatchStats GetPatchStatus();
//
// 	UFUNCTION()
// 	void GetDiskTotalAndFreeSpace(const FString& InPath, uint64& TotalSpace, uint64& FreeSpace);
//
// public:
// 	UPROPERTY()
// 	FPatchCompleteDelegate OnPatchComplete;
//
// protected:
// 	FString PatchPlatform;
// 	
	UPROPERTY(EditDefaultsOnly)
	FString PatchVersionURL; // "http://10.10.11.159:18080/PatchVersion.json"
//
// 	bool bIsDownloadManifestUpToDate;
//
	UPROPERTY(EditDefaultsOnly)
	TArray<int32> ChunkDownloadList;
// 	/////////////////// ChunkDownloader /////////////////// end
	
};
