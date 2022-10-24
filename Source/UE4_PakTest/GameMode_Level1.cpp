// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode_Level1.h"


// Sets default values
AGameMode_Level1::AGameMode_Level1()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;
}

void AGameMode_Level1::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	
	// 3D 물체등을 그린다.
	UE_LOG(LogTemp, Log, TEXT("hihi"));

	if(IsValid(UIMainClass))
	{
		UGameInstance* GameInstance = GetWorld()->GetGameInstance();
		UIMain = Cast<UUIMain_Level1>(CreateWidget(GameInstance, UIMainClass, TEXT("UIMain")));
	}

	if(IsValid(UIMain))
	{
		UIMain->AddToViewport(0);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT(""));
	}
}
