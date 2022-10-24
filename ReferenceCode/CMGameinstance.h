// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Tickable.h"
#include "UICommon.h"
#include "Engine/GameInstance.h"
#include "Engine/StreamableManager.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "CMGameInstance.generated.h"


enum class ETB_Skill_Slot : uint8;
enum class EGameMode;

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLoadingScreenLoadedSignature);
DECLARE_MULTICAST_DELEGATE(FOnLevelLoaded);
DECLARE_DELEGATE(FOnPreLoadedDelegate);

///////////////////////////////////////////////////////////////////////////////////////////////
// ChunkDownloader
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPatchCompleteDelegate, const bool, Succeeded);

USTRUCT()
struct FPPatchStats
{
	GENERATED_BODY()

	UPROPERTY()
	int32 FilesDownloaded = 0;

	UPROPERTY()
	int32 TotalFilesToDownload = 0;

	UPROPERTY()
	float DownloadPercent = 0.f;

	UPROPERTY()
	int32 MBDownloaded = 0;

	UPROPERTY()
	int32 TotalMBToDownload = 0;

	UPROPERTY()
	FText LastError;
};


class UCMItemInfoManager;


/**
 *
 */
UCLASS()
class CM_API UCMGameInstance : public UGameInstance, public FTickableGameObject
{
	GENERATED_BODY()

	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return HasAnyFlags(RF_ClassDefaultObject) == false; }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UCMGameInstance, STATGROUP_Tickables); }

public:
	UCMGameInstance();
	virtual void Init() override;			// 초기시 호출한다
	virtual void Shutdown() override;
	virtual void FinishDestroy() override;	// 종료시 호출한다

	virtual void OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld) override;

	UFUNCTION() void SetCurrentGameMode(EGameMode eGameMode);									 // 게임 씬정보 설정

	// UFUNCTION(BlueprintCallable, Category = "OpenLevel")
	bool IsValidLevel(const FName& InLevelPath);

	/**
	 * @brief 레벨 이동 함수
	 * @param InLevelPath 레벨 경로
	 * @param InIsRestorePageHistory 레벨 이동 후 페이지 히스토리 복원 여부
	 * @param IsAsyncLoad 레벨 비동기 로드 여부
	 * @param InLevelOptions 레벨 이동 옵션
	 * @param In_E_PanelTransitionType 레벨 전환효과 타입
	 * @param IsStorePageHistory_From_CurrentLevel 현재 레벨의 히스토리를 저장할지 여부, 뒤로가기 시 false하는게 좋다.(현재 레벨과 다음 레벨이 같을 경우 이전 레벨과 다음 레벨이 같아져 뒤로가기가 안되는 현상 방지)
	 */
	void OpenLevel(FName& InLevelPath/*todo #to FString*/, bool InIsRestorePageHistory = false, bool IsAsyncLoad = true, TMap<FString, FString> InLevelOptions = {}, EUI_PanelTransitionType In_E_PanelTransitionType = EUI_PanelTransitionType::Black, const bool IsStorePageHistory_From_CurrentLevel = true); // 레벨열기
	
	void OpenLevel_By_GameModeType(EUI_GameModeType In_EUI_GameModeType, TMap<FString, FString> InLevelOptions = {}, bool InIsRestorePageHistory = false, bool IsAsyncLoad = true, EUI_PanelTransitionType In_E_PanelTransitionType = EUI_PanelTransitionType::Black, const bool IsStorePageHistory_From_CurrentLevel = true); // 레벨열기
	void OpenPrevLevel_By_GameModeClass(UClass* InGameModeClass, TMap<FString, FString> InLevelOptions = {}, bool InIsRestorePageHistory = false, bool IsAsyncLoad = true, EUI_PanelTransitionType In_E_PanelTransitionType = EUI_PanelTransitionType::Black); // 레벨열기
	
	void OpenLevel_To_SkillPriview(uint32 CharID, ETB_Skill_Slot In_E_Skill_Slot); // 레벨열기 - 스킬 프리뷰

	const bool HasLevelOption(const class AGameMode* const InGameMode, const FString InKey);
	const FString GetLevelOptionValue(const class AGameMode* const InGameMode, const FString InKey);
	
	UFUNCTION(BlueprintCallable, Category = "QuitGame")	 void QuitGame();						 // 게임종료
	UFUNCTION(BlueprintCallable, Category = "LoadStLevel") void LoadStreamLevel(FName& level);	 // 스트림 레벨 열기
	UFUNCTION(BlueprintCallable, Category = "UnStLdLevel") void UnLoadStreamLevel(FName& level); // 스트림 레벨 닫기

	UFUNCTION() void SuccessLdStreamLevel();
	UFUNCTION() void SuccessUlStreamLevel();
	UFUNCTION() bool GetIsLoadStreamLevel(FName& level);
	UFUNCTION() bool IsStreamLevel(FName& level);

	UFUNCTION() class APlayerController* GetFirstLocalPlayerController(UWorld* World = nullptr) const;

public:
	UFUNCTION()	EGameMode GetCurrentGameMode() { return CurrentGameMode; }									// 씬정보를 가져온다.

	UFUNCTION() inline class UCMSoundManager* GetSoundManager()					{ return SoundManager; }		// 사운드 메니져를 만든다
	UFUNCTION()	inline class UNetManager* GetNetManager()						{ return NetManager; }		// 네트워크 매니저 가져오는 함수
	// Deprecated, todo #server
	UFUNCTION() inline class UCMTimeManager* GetTimeManager()					{ return TimeManager; }			// 게임 플레이타임 관리
	UFUNCTION() inline class UCMServerTimeManager* GetServerTimeManager()		{ return ServerTimeManager; }	// 서버 시간 및 로컬 시간 관리 
	UFUNCTION() inline class UCMAccountManager* GetAccountManager()				{ return AccountManager; }		// DB 계정 정보 : 재화 정보 포함
	UFUNCTION() inline class UCMItemInfoManager* GetItemInfoManager()			{ return ItemInfoManager; }		// DB 아이템 정보
	// UFUNCTION()	inline class UCMCharInfoManager* GetCharInfoManager()			{ return CharInfoManager; }		// DB 캐릭터 정보 : 덱 정보 포함
	UFUNCTION()	inline class UCMCharInfoManager* GetCharInfoManager()			{ return CharInfoManager; }	// DB 캐릭터 정보 : 덱 정보 포함
	UFUNCTION() inline class UCMStageInfoManager* GetStageInfoManager()			{ return StageInfoManager; }	// DB 스테이지 정보
	UFUNCTION() inline class UCMFacilityInfoManager* GetFacilityInfoManager()	{ return FacilityInfoManager; }	// DB 시설 정보
	UFUNCTION() inline class UCMTacticInfoManager* GetTacticInfoManager()		{ return TacticInfoManager; }	// 전술 정보를 관리합니다.
	UFUNCTION() inline class UCMGameActorManager* GetGameActorManager()			{ return GameActorManager; }
	UFUNCTION()	inline class UCMUISystemManager* GetUISystemManager()			{ return UISystemManager; }		// UI 시스템
	UFUNCTION() inline class UYuriToolManager* GetYuriToolManager()				{ return YuriToolManager; }
	UFUNCTION() inline class UCMTutorialManager* GetTutorialManager()			{ return TutorialManager; }

	FOnLevelLoaded OnNewLevelLoaded;
	FOnLevelLoaded OnNewLevelLoadedOnce;

	UFUNCTION() void LoadLevel_Complete();
	UFUNCTION() void InternalOpenLevel();
	
	UPROPERTY() FString LevelPath; // todo #to FName
	UPROPERTY() FString LevelOptionString;
	FSoftObjectPath MapToLoad;
	FStreamableManager Loader;
	FTimerHandle handle;
	
	// UPROPERTY() FName GroundStreamPath;	// 스트리밍 레벨이름을 가지고있다

	UFUNCTION()
	const bool IsPlayStandAlone() const { return PlayStandAlone; }

	static TMap<EUI_GameModeType, TFunction<bool(FName& OutPath, TMap<FString, FString>& OutLevelOptions, UCMGameInstance* InGameInstance)>> OpenLevelPathPrevHandlers;

private:
	UFUNCTION() void InitManager();
	UFUNCTION() void InitConsoleCommands();

	UPROPERTY() EGameMode CurrentGameMode;
	UPROPERTY() class UNetManager* NetManager = nullptr;
	UPROPERTY()	class UCMSoundManager* SoundManager = nullptr;
	UPROPERTY() class UCMTimeManager* TimeManager = nullptr;
	UPROPERTY() class UCMServerTimeManager* ServerTimeManager = nullptr;
	UPROPERTY() class UCMAccountManager* AccountManager = nullptr;
	UPROPERTY() class UCMItemInfoManager* ItemInfoManager = nullptr;
	UPROPERTY()	class UCMCharInfoManager* CharInfoManager = nullptr;
	UPROPERTY() class UCMStageInfoManager* StageInfoManager = nullptr;
	UPROPERTY() class UCMFacilityInfoManager* FacilityInfoManager = nullptr;
	UPROPERTY() class UCMTacticInfoManager* TacticInfoManager = nullptr;
	UPROPERTY() class UCMGameActorManager* GameActorManager = nullptr;
	UPROPERTY()	class UCMUISystemManager* UISystemManager = nullptr;
	UPROPERTY() class UYuriToolManager* YuriToolManager = nullptr;
	UPROPERTY() class UCMTutorialManager* TutorialManager = nullptr;
	
	UPROPERTY()	class UCMConsoleCommands* ConsoleCommands = nullptr;
	static void OnConsoleCommandWithArgs(const TArray<FString>& InArgs, UWorld* InWorld);

	UPROPERTY()
	bool PlayStandAlone = false;

	UFUNCTION()
	void CheckPlayStandAlone();



///////////////////////////////////////////////////////////////////////////////////////////////
// ChunkDownloader
protected:
	void InitPatchingSystem(const FString& InPatchPlatform);
	void OnPatchingSystemVersionResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void InitChunkDownloader(const FString& BuildID);
	void OnManifestUpdateComplete(const bool bSuccess);
	void OnDownloadComplete(const bool bSuccess);
	void OnLoadingModeComplete(const bool bSuccess);
	void OnMountComplete(const bool bSuccess);

public:
	UFUNCTION()
	void GetLoadingProgress(int32& BytesDownloaded, int32& TotalBytesToDownload, float& DownloadPercent, int32& ChunkMounted, int32& TotalChunksToMount, float& MountPercent) const;

	UFUNCTION()
	bool PatchGame();

	UFUNCTION()
	const int32 GetChunkDownloaderListCount() const { return ChunkDownloadList.Num(); }

	UFUNCTION()
	FPPatchStats GetPatchStatus();

	UFUNCTION()
	void GetDiskTotalAndFreeSpace(const FString& InPath, uint64& TotalSpace, uint64& FreeSpace);

public:
	UPROPERTY()
	FPatchCompleteDelegate OnPatchComplete;

protected:
	FString PatchPlatform;
	
	UPROPERTY(EditDefaultsOnly)
	FString PatchVersionURL;

	bool bIsDownloadManifestUpToDate;

	UPROPERTY(EditDefaultsOnly)
	TArray<int32> ChunkDownloadList;
};
