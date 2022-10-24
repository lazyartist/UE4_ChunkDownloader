// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UMG.h"

#include "UIMain_Level0.generated.h"

/**
 * 
 */
UCLASS()
class UE4_PAKTEST_API UUIMain_Level0 : public UUserWidget
{
	GENERATED_BODY()

	void NativePreConstruct() override;

	UPROPERTY(meta=(BindWidgetOptional))
	UButton* DownloadButton_BP = nullptr;

	UPROPERTY(meta=(BindWidgetOptional))
	UButton* NextLevelButton_BP = nullptr;

	UFUNCTION()
	void DownloadButton_OnClicked();

	UFUNCTION()
	void NextLevelButton_OnClicked();

public:
	UFUNCTION(BlueprintCallable)
	void RequestContent_OnSucceeded(UObject* MobilePendingContent);
	
	UFUNCTION(BlueprintCallable)
	void RequestContent_OnFailed(FText ErrorText, int32 ErrorCode);
};
