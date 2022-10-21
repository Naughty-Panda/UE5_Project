// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "BartenderEnder/Character/PawnData.h"
#include "BartenderEnder/LogCategories.h"
#include "GameAssetManager.generated.h"

/**
 * Default Asset Manager class for Bartender Ender.
 */
UCLASS(Config = Game)
class UGameAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	UGameAssetManager();

	static UGameAssetManager& Get();

	// Returns Default Pawn Data from project settings.
	const UPawnData* GetDefaultPawnData() const;
	
	// Synchronously loads and returns the asset provided by TSoftObjectPtr.
	template <typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPtr, bool bKeepInMemory = true);

protected:
	virtual void StartInitialLoading() override;

	template <typename AssetType>
	static AssetType* LoadAssetSynchronously(const FSoftObjectPath& AssetPath);

	void AddLoadedAsset(const UObject* Asset);

private:
	// Game-wide ability system initialization.
	void InitializeAbilitySystem();

protected:
	// Default PawnData for the game. Used when we cannot get one from GamePlayerState.
	UPROPERTY(Config)
	TSoftObjectPtr<UPawnData> DefaultPawnData = nullptr;

private:
	// Array of loaded and tracked assets.
	UPROPERTY()
	TSet<const UObject*> LoadedAssets;

	// Critical section for a scope lock to modify list of loaded assets.
	FCriticalSection LoadedAssetsCritical;
};

template <typename AssetType>
AssetType* UGameAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPtr, bool bKeepInMemory)
{
	const FSoftObjectPath& AssetPath = AssetPtr.ToSoftObjectPath();

	AssetType* LoadedAsset = nullptr;

	if (AssetPath.IsValid())
	{
		LoadedAsset = AssetPtr.Get();

		if (!LoadedAsset)
		{
			LoadedAsset = LoadAssetSynchronously<AssetType>(AssetPath);
			ensureAlwaysMsgf(LoadedAsset, TEXT("Cannot load asset %s"), *AssetPtr.ToString());
		}

		if (LoadedAsset && bKeepInMemory)
		{
			Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
		}
	}

	return LoadedAsset;
}

template <typename AssetType>
AssetType* UGameAssetManager::LoadAssetSynchronously(const FSoftObjectPath& AssetPath)
{
	if (!AssetPath.IsValid())
	{
		UE_LOG(LogGameAssetManager, Error, TEXT("Cannot load asset: incorrect asset path!"));
		return nullptr;
	}

	TUniquePtr<FScopeLogTime> LogTimePtr;
	LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("Synchronously loaded asset: %s"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);

	if (UAssetManager::IsValid())
	{
		return UAssetManager::GetStreamableManager().LoadSynchronous<AssetType>(AssetPath, false);
	}

	// If AssetManager isn't ready yet try to use LoadObject.
	return Cast<AssetType>(AssetPath.TryLoad());
}
