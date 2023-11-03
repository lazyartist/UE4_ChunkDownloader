// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode_Level1.h"


AGameMode_Level1::AGameMode_Level1()
{
}

void AGameMode_Level1::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	
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
