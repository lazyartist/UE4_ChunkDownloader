// Fill out your copyright notice in the Description page of Project Settings.


#include "UIMain_Level0.h"

void UUIMain_Level0::NativePreConstruct()
{
	Super::NativePreConstruct();

	if(IsValid(DownloadButton_BP))
	{
		DownloadButton_BP->OnClicked.AddDynamic(this, &UUIMain_Level0::DownloadButton_OnClicked);
	}

	if(IsValid(NextLevelButton_BP))
	{
		NextLevelButton_BP->OnClicked.AddDynamic(this, &UUIMain_Level0::DownloadButton_OnClicked);
	}
}

void UUIMain_Level0::DownloadButton_OnClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Clicked"));

	// UMobilePatchingLibrary::RequestContent
	// RemoteManifestURL: http://localhost/files/ManifestDir/UE4_PakTest_pakchunk1release1.manifest
	// CloudURL: D:\www\files\CloudDir
	// InstallDirectory: InstallDirectory

	// adb logcat -s UE4 -s debug -s DEBUG
}

void UUIMain_Level0::NextLevelButton_OnClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Level1"), true);
}

void UUIMain_Level0::RequestContent_OnSucceeded(UObject* MobilePendingContent)
{
	UE_LOG(LogTemp, Log, TEXT("RequestContent_OnSucceeded"));
}

void UUIMain_Level0::RequestContent_OnFailed(FText ErrorText, int32 ErrorCode)
{
	UE_LOG(LogTemp, Log, TEXT("RequestContent_OnFailed"));
}
