// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UMG.h"
#include "CMChunkDownloader.h"

#include "UIMain_Level0.generated.h"

/**
 * 
 */
UCLASS()
class UE4_PAKTEST_API UUIMain_Level0 : public UUserWidget
{
	GENERATED_BODY()

	void NativePreConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	FString ChunkDownloaderStateLog;
	
	UPROPERTY(meta=(BindWidgetOptional))
	UTextBlock* ChunkDownloaderStateText_BP = nullptr;
	
	UPROPERTY(meta=(BindWidgetOptional))
	UTextBlock* ChunkDownloaderProgressText_BP = nullptr;

	EChunkDownloaderState mEChunkDownloaderState;

	UPROPERTY()
	class UCMChunkDownloader* mChunkDownloader;

	UFUNCTION()
	void DownloadButton_OnClicked();

	UFUNCTION(BlueprintCallable)
	void DownloadChunk();

	UFUNCTION(BlueprintCallable)
	void DownloadChunk_With_PatchVersionURL(const FString& InBuildID);

	UFUNCTION()
	void UpdateChunkDownloaderProgress(const FChunkDownloaderProgress& InChunkDownloaderProgress);

	UFUNCTION()
	void InternalDownloadChunkData();

	UFUNCTION()
	void OnChunkDownloaderState_Update(const EChunkDownloaderState InEChunkDownloaderState);

	UFUNCTION()
	void OnChunkDownloaderProgress_Update(const FChunkDownloaderProgress& InChunkDownloaderProgress);
};

