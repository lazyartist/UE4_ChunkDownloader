// Fill out your copyright notice in the Description page of Project Settings.


#include "UIMain.h"

void UUIMain::NativePreConstruct()
{
	Super::NativePreConstruct();

	if(IsValid(DownloadButton_BP))
	{
		DownloadButton_BP->OnClicked.AddDynamic(this, &UUIMain::OnClicked);
	}
}

void UUIMain::OnClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Clicked"));

	// UMobilePatchingLibrary::RequestContent
	// RemoteManifestURL: http://localhost/files/ManifestDir/UE4_PakTest_pakchunk1release1.manifest
	// CloudURL: D:\www\files\CloudDir
	// InstallDirectory: InstallDirectory

	// adb logcat -s UE4 -s debug -s DEBUG
}

void UUIMain::RequestContent_OnSucceeded(UObject* MobilePendingContent)
{
	UE_LOG(LogTemp, Log, TEXT("RequestContent_OnSucceeded"));
}

void UUIMain::RequestContent_OnFailed(FText ErrorText, int32 ErrorCode)
{
	UE_LOG(LogTemp, Log, TEXT("RequestContent_OnFailed"));
}
