// Bartender Ender
// Naughty Panda @ 2022

#include "GameSettings.h"
#include "BartenderEnder/LogCategories.h"

UGameSettings::UGameSettings()
{
	RegisteredInputConfigs.Reset();
}

UGameSettings& UGameSettings::Get()
{
	// return by pointer
	//return GEngine ? CastChecked<UGameSettings>(GEngine->GetGameUserSettings()) : nullptr;

	check(GEngine);

	UGameSettings* GameSettings = Cast<UGameSettings>(GEngine->GetGameUserSettings());

	if (!GameSettings)
	{
		// Crash here!
		UE_LOG(LogGameSettings, Fatal, TEXT("Cannot get GameSettings! Invalid GameSettings class in engine config!"));
	}

	return *GameSettings;
}

void UGameSettings::RegisterInputConfig(ECommonInputType Type, const UPlayerMappableInputConfig* NewConfig, bool bIsActive)
{
	if (!NewConfig)
	{
		UE_LOG(LogGameSettings, Error, TEXT("Cannot register input config with NULL config!"));
		return;
	}

	const int32 RegisteredConfigIndex = GetRegisteredConfigIndex(NewConfig);

	if (RegisteredConfigIndex != INDEX_NONE)
	{
		UE_LOG(LogGameSettings, Error, TEXT("Input config had already been registered!"));
		return;
	}

	if (const int32 AddedConfigIndex = RegisteredInputConfigs.Add(FLoadedMappableConfigContainer(NewConfig, Type, bIsActive)); AddedConfigIndex != INDEX_NONE)
	{
		UE_LOG(LogGameSettings, Log, TEXT("New input config successfully registered!"));
		OnInputConfigRegistered.Broadcast(RegisteredInputConfigs[AddedConfigIndex]);
	}
}

int32 UGameSettings::UnregisterInputConfig(const UPlayerMappableInputConfig* Config)
{
	if (!Config)
	{
		UE_LOG(LogGameSettings, Error, TEXT("Cannot unregister NULL config!"));
		return INDEX_NONE;
	}

	const int32 RegisteredConfigIndex = GetRegisteredConfigIndex(Config);

	if (RegisteredConfigIndex != INDEX_NONE)
	{
		RegisteredInputConfigs.RemoveAt(RegisteredConfigIndex);
		return 1;
	}

	return INDEX_NONE;
}

void UGameSettings::ActivateInputConfig(const UPlayerMappableInputConfig* Config)
{
	if (!Config)
	{
		UE_LOG(LogGameSettings, Error, TEXT("Cannot activate NULL config!"));
		return;
	}

	const int32 RegisteredConfigIndex = GetRegisteredConfigIndex(Config);

	if (RegisteredConfigIndex == INDEX_NONE)
	{
		UE_LOG(LogGameSettings, Error, TEXT("Input config was not registered. Cannot activate it!"));
		return;
	}

	RegisteredInputConfigs[RegisteredConfigIndex].bIsActive = true;
	OnInputConfigActivated.Broadcast(RegisteredInputConfigs[RegisteredConfigIndex]);
}

void UGameSettings::DeactivateInputConfig(const UPlayerMappableInputConfig* Config)
{
	if (!Config)
	{
		UE_LOG(LogGameSettings, Error, TEXT("Cannot deactivate NULL config!"));
		return;
	}

	const int32 RegisteredConfigIndex = GetRegisteredConfigIndex(Config);

	if (RegisteredConfigIndex == INDEX_NONE)
	{
		UE_LOG(LogGameSettings, Error, TEXT("Input config was not registered. Cannot deactivate it!"));
		return;
	}

	RegisteredInputConfigs[RegisteredConfigIndex].bIsActive = false;
	OnInputConfigDeactivated.Broadcast(RegisteredInputConfigs[RegisteredConfigIndex]);
}

int32 UGameSettings::GetRegisteredConfigIndex(const UPlayerMappableInputConfig* const Config) const
{
	return RegisteredInputConfigs.IndexOfByPredicate(
		[&Config](const FLoadedMappableConfigContainer& Container) -> bool
		{
			return Container.InputConfig == Config;
		}
	);
}
