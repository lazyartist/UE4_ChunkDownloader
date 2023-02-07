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

	// UPROPERTY(meta=(BindWidgetOptional))
	// UButton* DownloadButton_BP = nullptr;
	
	FString ChunkDownloaderStateLog;
	
	UPROPERTY(meta=(BindWidgetOptional))
	UTextBlock* ChunkDownloaderStateText_BP = nullptr;
	
	UPROPERTY(meta=(BindWidgetOptional))
	UTextBlock* ChunkDownloaderProgressText_BP = nullptr;


	// UPROPERTY(meta=(BindWidgetOptional))
	// UButton* Download0Button_BP = nullptr;
	//
	// UPROPERTY(meta=(BindWidgetOptional))
	// UButton* Download1Button_BP = nullptr;
	//
	// UPROPERTY(meta=(BindWidgetOptional))
	// UButton* Download2Button_BP = nullptr;
	//
	// UPROPERTY(meta=(BindWidgetOptional))
	// UButton* OpenLevel0Button_BP = nullptr;
	//
	// UPROPERTY(meta=(BindWidgetOptional))
	// UButton* OpenLevel1Button_BP = nullptr;
	//
	// UPROPERTY(meta=(BindWidgetOptional))
	// UButton* OpenLevel2Button_BP = nullptr;

	// UPROPERTY(EditAnywhere)
	// TArray<FString> mLevelNames;

	UFUNCTION()
	void DownloadButton_OnClicked();

	UFUNCTION(BlueprintCallable)
	void DownloadChunk();

	UFUNCTION(BlueprintCallable)
	void DownloadChunk_With_PatchVersionURL(const FString& InBuildID);

	UFUNCTION()
	void UpdateChunkDownloaderProgress(const FChunkDownloaderProgress& InChunkDownloaderProgress);

	EChunkDownloaderState mEChunkDownloaderState;

	UPROPERTY()
	class UCMChunkDownloader* mChunkDownloader;

	UFUNCTION()
	void InternalDownloadChunkData();

	UFUNCTION()
	void OnChunkDownloaderState_Update(const EChunkDownloaderState InEChunkDownloaderState);

	UFUNCTION()
	void OnChunkDownloaderProgress_Update(const FChunkDownloaderProgress& InChunkDownloaderProgress);
	
	//
	// UFUNCTION()
	// void Download0Button_OnClicked();
	//
	// UFUNCTION()
	// void OpenLevel0Button_OnClicked();
	//
	// UFUNCTION()
	// void OpenLevel1Button_OnClicked();
	//
	// UFUNCTION()
	// void OpenLevel2Button_OnClicked();
	//
	// UFUNCTION()
	// void OpenLevel(const FString In_LevelName);

public:
	// UFUNCTION(BlueprintCallable)
	// void RequestContent_OnSucceeded(UObject* MobilePendingContent);
	//
	// UFUNCTION(BlueprintCallable)
	// void RequestContent_OnFailed(FText ErrorText, int32 ErrorCode);
};

