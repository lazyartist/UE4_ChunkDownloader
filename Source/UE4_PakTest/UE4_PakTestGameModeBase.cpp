// Copyright Epic Games, Inc. All Rights Reserved.


#include "UE4_PakTestGameModeBase.h"

void AUE4_PakTestGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// 3D 물체등을 그린다.
	UE_LOG(LogTemp, Log, TEXT("hihi"));

	
}
