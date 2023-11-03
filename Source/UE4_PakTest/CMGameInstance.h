// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IHttpRequest.h"
#include "CMGameInstance.generated.h"

UCLASS()
class UE4_PAKTEST_API UCMGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UCMGameInstance();

	UPROPERTY(EditDefaultsOnly)
	FString PatchVersionURL; // "http://192.168.0.10:9090/PatchVersion.json"

	UPROPERTY(EditDefaultsOnly)
	TArray<int32> ChunkDownloadList;
	
};
