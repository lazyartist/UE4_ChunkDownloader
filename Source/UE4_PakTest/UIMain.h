// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UMG.h"

#include "UIMain.generated.h"

/**
 * 
 */
UCLASS()
class UE4_PAKTEST_API UUIMain : public UUserWidget
{
	GENERATED_BODY()

	void NativePreConstruct() override;

	UPROPERTY(meta=(BindWidgetOptional))
	UButton* DownloadButton_BP = nullptr;

	UFUNCTION()
	void OnClicked();
};
