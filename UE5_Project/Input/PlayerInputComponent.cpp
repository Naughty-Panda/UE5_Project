// Bartender Ender
// Naughty Panda @ 2022

#include "PlayerInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputConfig.h"
#include "MappableConfigContainer.h"
#include "BartenderEnder/Settings/GameSettings.h"

void UPlayerInputComponent::AddInputMappings(const UInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	const UGameSettings& GameSettings = UGameSettings::Get();

	const TArray<FLoadedMappableConfigContainer>& Configs = GameSettings.GetRegisteredInputConfigs();
	for (const FLoadedMappableConfigContainer& ConfigContainer : Configs)
	{
		if (ConfigContainer.bIsActive)
		{
			// TODO: Options for config?
			InputSubsystem->AddPlayerMappableConfig(ConfigContainer.InputConfig);
		}
	}

	// TODO: Custom keymaps.
}

void UPlayerInputComponent::RemoveInputMappings(const UInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	const UGameSettings& GameSettings = UGameSettings::Get();

	const TArray<FLoadedMappableConfigContainer>& Configs = GameSettings.GetRegisteredInputConfigs();
	for (const FLoadedMappableConfigContainer& ConfigContainer : Configs)
	{
		if (ConfigContainer.bIsActive)
		{
			InputSubsystem->RemovePlayerMappableConfig(ConfigContainer.InputConfig);
		}
	}

	// TODO: Remove custom keymaps.
}

void UPlayerInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (const uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}

	BindHandles.Reset();
}

void UPlayerInputComponent::AddInputConfig(const FLoadedMappableConfigContainer& ConfigContainer, UEnhancedInputLocalPlayerSubsystem* InputSubsystem)
{
	check(InputSubsystem);

	if (ensure(ConfigContainer.bIsActive))
	{
		InputSubsystem->AddPlayerMappableConfig(ConfigContainer.InputConfig);
	}
}

void UPlayerInputComponent::RemoveInputConfig(const FLoadedMappableConfigContainer& ConfigContainer, UEnhancedInputLocalPlayerSubsystem* InputSubsystem)
{
	check(InputSubsystem);

	if (ConfigContainer.bIsActive)
	{
		return;
	}

	InputSubsystem->RemovePlayerMappableConfig(ConfigContainer.InputConfig);
}
