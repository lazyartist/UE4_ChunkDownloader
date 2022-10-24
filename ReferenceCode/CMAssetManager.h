#pragma once


#include "Engine/AssetManager.h"
#include "CMAssetManager.generated.h"



UCLASS()
class CM_API UCMAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static const FPrimaryAssetType	PlayerDataType;
	static const FPrimaryAssetType	MonsterDataType;
	static const FPrimaryAssetType	BuildingType;
	static const FPrimaryAssetType	UserWidgetType;
	static const FPrimaryAssetType	SkeletalMeshDataType;

public:
	virtual void StartInitialLoading() override;

	static UCMAssetManager& Get();
	class UCMAssetData* TryLoadSoftObjectPath(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning = true);

	TSharedPtr<FStreamableHandle> TryLoadPrimaryAsset(const FPrimaryAssetId& AssetToLoad);
	TSharedPtr<FStreamableHandle> TryLoadPrimaryAssets(const TArray<FPrimaryAssetId>& AssetsToLoad);
	TSharedPtr<FStreamableHandle> TryLoadPrimaryAssetsWithType(FPrimaryAssetType PrimaryAssetType);

	void OnAssetDataLoaded(FPrimaryAssetId LoadedID);
	void OnAssetDatasLoaded(TArray<FPrimaryAssetId> LoadedIDs);
	void OnAssetDataTypeLoaded(FPrimaryAssetType LoadedType);

	bool TyrLoadPrimaryAssetSoftObjectPath(const FPrimaryAssetId& AssetToLoad);
	bool TryLoadPrimaryAssetsSoftObjectPathWithType(FPrimaryAssetType PrimaryAssetType);

	void OnCompleteAsyncLoadByAssetData(FSoftObjectPath InAssetSOPath);
	void OnCompleteAsyncLoadByAssetDataList(TArray<FSoftObjectPath> InAssetSOPathList);

	TSharedPtr<FStreamableHandle> TryLoadPrimaryAssetDelegateToCall(const FPrimaryAssetId& AssetToLoad, FStreamableDelegate DelegateToCall = FStreamableDelegate());
	TSharedPtr<FStreamableHandle> TryLoadPrimaryAssetsDelegateToCall(const TArray<FPrimaryAssetId>& AssetsToLoad, FStreamableDelegate DelegateToCall = FStreamableDelegate());
	TSharedPtr<FStreamableHandle> TryLoadPrimaryAssetWithTypeDelegateToCall(FPrimaryAssetType PrimaryAssetType, FStreamableDelegate DelegateToCall = FStreamableDelegate());


private:
	//Pawn 안찾고 바로 넣을랭
	UPROPERTY()
	class ACMPlayerPawn* PlayerPawn = nullptr;
};