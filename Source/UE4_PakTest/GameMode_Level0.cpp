// Copyright Epic Games, Inc. All Rights Reserved.


#include "GameMode_Level0.h"

void AGameMode_Level0::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	if(IsValid(UIMainClass))
	{
		UGameInstance* GameInstance = GetWorld()->GetGameInstance();
		UIMain = Cast<UUIMain_Level0>(CreateWidget(GameInstance, UIMainClass, TEXT("UIMain")));
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
