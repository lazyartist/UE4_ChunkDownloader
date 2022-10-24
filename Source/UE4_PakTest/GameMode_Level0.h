// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UIMain_Level0.h"
#include "GameFramework/GameModeBase.h"
#include "GameMode_Level0.generated.h"

/**
 * 
 */
UCLASS()
class UE4_PAKTEST_API AGameMode_Level0 : public AGameModeBase
{
	GENERATED_BODY()

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUIMain_Level0> UIMainClass;

	UPROPERTY(EditAnywhere)
	UUIMain_Level0* UIMain; 
};
