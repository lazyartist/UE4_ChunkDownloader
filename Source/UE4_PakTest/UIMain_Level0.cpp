// Fill out your copyright notice in the Description page of Project Settings.


#include "UIMain_Level0.h"

void UUIMain_Level0::NativePreConstruct()
{
	Super::NativePreConstruct();

	// if(IsValid(Download0Button_BP))
	// {
	// 	Download0Button_BP->OnClicked.AddDynamic(this, &UUIMain_Level0::Download0Button_OnClicked);
	// }
	//
	// if(IsValid(Download1Button_BP))
	// {
	// 	Download1Button_BP->OnClicked.AddDynamic(this, &UUIMain_Level0::Download0Button_OnClicked);
	// }
	//
	// if(IsValid(Download2Button_BP))
	// {
	// 	Download2Button_BP->OnClicked.AddDynamic(this, &UUIMain_Level0::Download0Button_OnClicked);
	// }
	//
	// if(IsValid(OpenLevel0Button_BP))
	// {
	// 	OpenLevel0Button_BP->OnClicked.AddDynamic(this, &UUIMain_Level0::OpenLevel0Button_OnClicked);
	// }
	//
	// if(IsValid(OpenLevel1Button_BP))
	// {
	// 	OpenLevel1Button_BP->OnClicked.AddDynamic(this, &UUIMain_Level0::OpenLevel1Button_OnClicked);
	// }
	//
	// if(IsValid(OpenLevel2Button_BP))
	// {
	// 	OpenLevel2Button_BP->OnClicked.AddDynamic(this, &UUIMain_Level0::OpenLevel2Button_OnClicked);
	// }
}

// void UUIMain_Level0::Download0Button_OnClicked()
// {
// 	UE_LOG(LogTemp, Log, TEXT("Clicked"));
//
// 	// UMobilePatchingLibrary::RequestContent
// 	// RemoteManifestURL: http://localhost/files/ManifestDir/UE4_PakTest_pakchunk1release1.manifest
// 	// CloudURL: D:\www\files\CloudDir
// 	// InstallDirectory: InstallDirectory
//
// 	// adb logcat -s UE4 -s debug -s DEBUG
// }

// void UUIMain_Level0::OpenLevel0Button_OnClicked()
// {
// 	OpenLevel(mLevelNames[0]);
// 	// OpenLevel(TEXT("StartAssets/Level0"));
// }
//
// void UUIMain_Level0::OpenLevel1Button_OnClicked()
// {
// 	OpenLevel(mLevelNames[1]);
// }
//
// void UUIMain_Level0::OpenLevel2Button_OnClicked()
// {
// 	OpenLevel(mLevelNames[2]);
// }

// void UUIMain_Level0::OpenLevel(const FString In_LevelName)
// {
// 	UE_LOG(LogTemp, Log, TEXT("OpenLevel %s"), *In_LevelName);
// 	
// 	UGameplayStatics::OpenLevel(GetWorld(), *In_LevelName, true);
// }

// void UUIMain_Level0::RequestContent_OnSucceeded(UObject* MobilePendingContent)
// {
// 	UE_LOG(LogTemp, Log, TEXT("RequestContent_OnSucceeded"));
// }
//
// void UUIMain_Level0::RequestContent_OnFailed(FText ErrorText, int32 ErrorCode)
// {
// 	UE_LOG(LogTemp, Log, TEXT("RequestContent_OnFailed"));
// }
