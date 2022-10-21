// Bartender Ender
// Naughty Panda @ 2022

#include "MappableConfigContainer.h"
#include "ICommonUIModule.h"
#include "CommonUISettings.h"
#include "BartenderEnder/Settings/GameSettings.h"
#include "BartenderEnder/System/GameAssetManager.h"

bool FMappableConfigContainer::CanBeActivated() const
{
	const FGameplayTagContainer& PlatformTags = ICommonUIModule::GetSettings().GetPlatformTraits();

	// Check if current platform has all necessary tags for this config to be activated.
	if (!DependentPlatformTags.IsEmpty() && PlatformTags.HasAll(DependentPlatformTags))
	{
		return false;
	}

	// Check if current platform has any excluded tags.
	if (!ExcludedPlatformTags.IsEmpty() && PlatformTags.HasAny(ExcludedPlatformTags))
	{
		return false;
	}

	return true;
}

bool FMappableConfigContainer::RegisterContainer(const FMappableConfigContainer& Container)
{
	UGameSettings& GameSettings = UGameSettings::Get();
	UGameAssetManager& AssetManager = UGameAssetManager::Get();

	const UPlayerMappableInputConfig* LoadedConfig = AssetManager.GetAsset(Container.InputConfig);
	if (!LoadedConfig)
	{
		UE_LOG(LogGameAssetManager, Error, TEXT("Failed to load input config!"));
		return false;
	}

	GameSettings.RegisterInputConfig(Container.InputType, LoadedConfig, false);
	return true;
}

bool FMappableConfigContainer::ActivateContainer(const FMappableConfigContainer& Container)
{
	UGameAssetManager& AssetManager = UGameAssetManager::Get();

	if (FMappableConfigContainer::RegisterContainer(Container) && Container.CanBeActivated())
	{
		UGameSettings& GameSettings = UGameSettings::Get();
		if (const UPlayerMappableInputConfig* LoadedConfig = AssetManager.GetAsset(Container.InputConfig))
		{
			GameSettings.ActivateInputConfig(LoadedConfig);
			return true;
		}
	}

	return false;
}
