// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UE4_PakTestGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class UE4_PAKTEST_API AUE4_PakTestGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

public:
	
};
