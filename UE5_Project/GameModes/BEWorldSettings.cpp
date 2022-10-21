// Bartender Ender
// Naughty Panda @ 2022

#include "BEWorldSettings.h"
#include "Engine/AssetManager.h"

ABEWorldSettings::ABEWorldSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FPrimaryAssetId ABEWorldSettings::GetDefaultGameDefinition() const
{
	FPrimaryAssetId Result;
	if (!DefaultGameDefinition.IsNull())
	{
		Result = UAssetManager::Get().GetPrimaryAssetIdForPath(DefaultGameDefinition.ToSoftObjectPath());

		if (!Result.IsValid())
		{
			UE_LOG(LogEngine, Error, TEXT("%s.DefaultGameDefinition is %s but that failed to resolve into an asset ID (you might need to add a path to the Asset Rules in your game feature plugin or project settings"),
				*GetPathNameSafe(this), *DefaultGameDefinition.ToString());
		}
	}
	return Result;
}
