// Bartender Ender
// Naughty Panda @ 2022

#include "GameAssetManager.h"
#include "AbilitySystemGlobals.h"
#include "BartenderEnder/CoreGameplayTags.h"

UGameAssetManager::UGameAssetManager()
{
}

UGameAssetManager& UGameAssetManager::Get()
{
	check(GEngine);

	UGameAssetManager* Manager = Cast<UGameAssetManager>(GEngine->AssetManager);

	if (!Manager)
	{
		// Crash here!
		UE_LOG(LogGameAssetManager, Fatal, TEXT("Cannot get GameAssetManager! Invalid AssetManager class in engine config!"));
	}

	return *Manager;
}

const UPawnData* UGameAssetManager::GetDefaultPawnData() const
{
	return GetAsset(DefaultPawnData);
}

void UGameAssetManager::StartInitialLoading()
{
	SCOPED_BOOT_TIMING("UGameAssetManager::StartInitialLoading");

	Super::StartInitialLoading();

	InitializeAbilitySystem();
}

void UGameAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (!Asset)
	{
		UE_LOG(LogGameAssetManager, Error, TEXT("Cannot add loaded asset! Asset = NULL!"));
		return;
	}

	FScopeLock LoadingLock(&LoadedAssetsCritical);
	LoadedAssets.Add(Asset);

	UE_LOG(LogGameAssetManager, Warning, TEXT("Asset %s added! Total Assets: %i"), *GetNameSafe(Asset), LoadedAssets.Num());
}

void UGameAssetManager::InitializeAbilitySystem()
{
	SCOPED_BOOT_TIMING("UGameAssetManager::InitializeAbilitySystem");

	FCoreGameplayTags::InitializeNativeTags();

	UAbilitySystemGlobals::Get().InitGlobalData();
}
