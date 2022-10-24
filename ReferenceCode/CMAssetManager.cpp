#include "CMAssetManager.h"
#include "CMAssetData.h"
#include "CMSkeletalMeshAssetData.h"
#include "CMPlayerAssetData.h"
#include "CMPlayerPawn.h"
#include "Engine/StreamableManager.h"


const FPrimaryAssetType	UCMAssetManager::PlayerDataType = TEXT("Player");
const FPrimaryAssetType	UCMAssetManager::MonsterDataType = TEXT("Monster");
const FPrimaryAssetType	UCMAssetManager::BuildingType = TEXT("Building");
const FPrimaryAssetType UCMAssetManager::UserWidgetType = TEXT("UI");
const FPrimaryAssetType UCMAssetManager::SkeletalMeshDataType = TEXT("SkeletalMesh");


UCMAssetManager& UCMAssetManager::Get()
{
	UCMAssetManager* const pThis = Cast<UCMAssetManager>(GEngine->AssetManager);
	if (pThis)
	{
		return *pThis;
	}
	else
	{
		// 예외처리로 넣어놨지만 여기 들어오면 안됨!!!
		UE_LOG(CM, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, Must be Registration UCMAssetManager!"));
		return *NewObject<UCMAssetManager>();
	}
}

void UCMAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

}

UCMAssetData* UCMAssetManager::TryLoadSoftObjectPath(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning)
{
	FSoftObjectPath AssetDataPath = GetPrimaryAssetPath(PrimaryAssetId);
	UCMAssetData* const LoadedAssetData = Cast<UCMAssetData>(AssetDataPath.TryLoad());

	if (bLogWarning && !LoadedAssetData)
	{
		UE_LOG(CM, Warning, TEXT("Failed to load AssetData for identifier %s!"), *PrimaryAssetId.ToString());
	}
	return LoadedAssetData;
}

TSharedPtr<FStreamableHandle> UCMAssetManager::TryLoadPrimaryAsset(const FPrimaryAssetId& AssetToLoad)
{
	TSharedPtr<FStreamableHandle> RetHandle;
	TArray<FName> BundleNames;
	FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &UCMAssetManager::OnAssetDataLoaded, AssetToLoad);
	RetHandle = LoadPrimaryAsset(AssetToLoad, BundleNames, Delegate, FStreamableManager::DefaultAsyncLoadPriority);
	return RetHandle;
}

TSharedPtr<FStreamableHandle> UCMAssetManager::TryLoadPrimaryAssets(const TArray<FPrimaryAssetId>& AssetsToLoad)
{
	TSharedPtr<FStreamableHandle> RetHandle;
	TArray<FName> BundleNames;
	FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &UCMAssetManager::OnAssetDatasLoaded, AssetsToLoad);
	RetHandle = LoadPrimaryAssets(AssetsToLoad, BundleNames, Delegate, FStreamableManager::DefaultAsyncLoadPriority);
	return RetHandle;
}

TSharedPtr<FStreamableHandle> UCMAssetManager::TryLoadPrimaryAssetsWithType(FPrimaryAssetType PrimaryAssetType)
{
	TSharedPtr<FStreamableHandle> RetHandle;
	TArray<FName> BundleNames;
	FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &UCMAssetManager::OnAssetDataTypeLoaded, PrimaryAssetType);
	RetHandle = LoadPrimaryAssetsWithType(PrimaryAssetType, BundleNames, Delegate, FStreamableManager::DefaultAsyncLoadPriority);
	return RetHandle;
}

void UCMAssetManager::OnAssetDataLoaded(FPrimaryAssetId LoadedID)
{
	UCMAssetData* const AssetData = Cast<UCMAssetData>(GetPrimaryAssetObject(LoadedID));
	if (AssetData)
	{
		
	}
	UnloadPrimaryAsset(LoadedID);
}

void UCMAssetManager::OnAssetDatasLoaded(TArray<FPrimaryAssetId> LoadedIDs)
{
	for (const FPrimaryAssetId& ID : LoadedIDs)
	{
		UCMAssetData* const AssetData = Cast<UCMAssetData>(GetPrimaryAssetObject(ID));
		if (AssetData)
		{

		}
	}
	UnloadPrimaryAssets(LoadedIDs);
}

void UCMAssetManager::OnAssetDataTypeLoaded(FPrimaryAssetType LoadedType)
{
	TArray<FPrimaryAssetId> AssetIDs;
	GetPrimaryAssetIdList(LoadedType, AssetIDs);

	for (const FPrimaryAssetId& ID : AssetIDs)
	{
		if (LoadedType == PlayerDataType)
		{
			UCMPlayerAssetData* const PlayerData = Cast<UCMPlayerAssetData>(GetPrimaryAssetObject(ID));
			if (PlayerData)
			{
				UWorld* const World = GEngine->GameViewport->GetWorld();
				PlayerPawn = World->SpawnActor<ACMPlayerPawn>(PlayerData->CharacterPawnClass, FVector(-630.f, -3220.f, 100.f), FRotator::ZeroRotator);
				if (PlayerPawn)
				{
					CM_LOG_SCR("Spawn Success!!!");
				}
			}
		}
	}
	UnloadPrimaryAssetsWithType(LoadedType);
}

bool UCMAssetManager::TyrLoadPrimaryAssetSoftObjectPath(const FPrimaryAssetId& AssetToLoad)
{
	bool Ret = false;
	FAssetData AssetData;
	Ret = GetPrimaryAssetData(AssetToLoad, AssetData);

	const FSoftObjectPath AssetSOPath = AssetData.ToSoftObjectPath();
	auto& StreamableManager = UAssetManager::GetStreamableManager();
	StreamableManager.RequestAsyncLoad(AssetSOPath, FStreamableDelegate::CreateUObject(this, &UCMAssetManager::OnCompleteAsyncLoadByAssetData, AssetSOPath));

	return Ret;
}

bool UCMAssetManager::TryLoadPrimaryAssetsSoftObjectPathWithType(FPrimaryAssetType PrimaryAssetType)
{
	bool Ret = false;
	TArray<FAssetData> AssetDatas;
	Ret = GetPrimaryAssetDataList(PrimaryAssetType, AssetDatas);

	TArray<FSoftObjectPath> AssetSOPathList;
	for (const FAssetData& Data : AssetDatas)
	{
		const FSoftObjectPath Path = Data.ToSoftObjectPath();
		AssetSOPathList.Emplace(Path);
	}
	auto& StreamableManager = UAssetManager::GetStreamableManager();
	StreamableManager.RequestAsyncLoad(AssetSOPathList, FStreamableDelegate::CreateUObject(this, &UCMAssetManager::OnCompleteAsyncLoadByAssetDataList, AssetSOPathList));

	return Ret;
}

void UCMAssetManager::OnCompleteAsyncLoadByAssetData(FSoftObjectPath InAssetSOPath)
{

}

void UCMAssetManager::OnCompleteAsyncLoadByAssetDataList(TArray<FSoftObjectPath> InAssetSOPathList)
{
	for (const FSoftObjectPath SoftObj : InAssetSOPathList)
	{
		auto& StreamableManager = UAssetManager::GetStreamableManager();
		UCMSkeletalMeshAssetData* const SkeletalMeshAssetData = Cast<UCMSkeletalMeshAssetData>(StreamableManager.LoadSynchronous(SoftObj));

		if (SkeletalMeshAssetData)
		{
			const TArray<USkeletalMesh*> AssetList = SkeletalMeshAssetData->SkeletalMeshes;
			TArray<FSoftObjectPath> AssetPath;
			for (USkeletalMesh* SkeletalMesh : AssetList)
			{
				AssetPath.Add(SkeletalMesh);
			}

			if (PlayerPawn)
			{
				PlayerPawn->SetPlayerModelMesh(AssetPath);
			}
		}
	}
}

TSharedPtr<FStreamableHandle> UCMAssetManager::TryLoadPrimaryAssetDelegateToCall(const FPrimaryAssetId& AssetToLoad, FStreamableDelegate DelegateToCall/* = FStreamableDelegate()*/)
{
	TSharedPtr<FStreamableHandle> RetHandle;
	TArray<FName> BundleNames;
	RetHandle = LoadPrimaryAsset(AssetToLoad, BundleNames, DelegateToCall, FStreamableManager::DefaultAsyncLoadPriority);

	return RetHandle;
}

TSharedPtr<FStreamableHandle> UCMAssetManager::TryLoadPrimaryAssetsDelegateToCall(const TArray<FPrimaryAssetId>& AssetsToLoad, FStreamableDelegate DelegateToCall/* = FStreamableDelegate()*/)
{
	TSharedPtr<FStreamableHandle> RetHandle;
	TArray<FName> BundleNames;
	RetHandle = LoadPrimaryAssets(AssetsToLoad, BundleNames, DelegateToCall, FStreamableManager::DefaultAsyncLoadPriority);

	return RetHandle;
}

TSharedPtr<FStreamableHandle> UCMAssetManager::TryLoadPrimaryAssetWithTypeDelegateToCall(FPrimaryAssetType PrimaryAssetType, FStreamableDelegate DelegateToCall/* = FStreamableDelegate()*/)
{
	TSharedPtr<FStreamableHandle> RetHandle;
	TArray<FName> BundleNames;
	RetHandle = LoadPrimaryAssetsWithType(PrimaryAssetType, BundleNames, DelegateToCall, FStreamableManager::DefaultAsyncLoadPriority);

	return RetHandle;
}