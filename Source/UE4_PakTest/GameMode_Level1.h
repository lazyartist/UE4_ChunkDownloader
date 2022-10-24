// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIMain_Level1.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameModeBase.h"
#include "GameMode_Level1.generated.h"

UCLASS()
class UE4_PAKTEST_API AGameMode_Level1 : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGameMode_Level1();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUIMain_Level1> UIMainClass;

	UPROPERTY(EditAnywhere)
	UUIMain_Level1* UIMain;
};
