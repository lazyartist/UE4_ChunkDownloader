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
}
