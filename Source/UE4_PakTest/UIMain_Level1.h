// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UMG.h"
#include "UObject/Object.h"
#include "UIMain_Level1.generated.h"

/**
 * 
 */
UCLASS()
class UE4_PAKTEST_API UUIMain_Level1 : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidgetOptional))
	UButton* PrevLevelButton_BP = nullptr;

	UFUNCTION()
	void PrevLevelButton_OnClicked();
};
