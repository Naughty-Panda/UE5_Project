// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "CommonInputBaseTypes.h"
#include "BartenderEnder/Input/MappableConfigContainer.h"
#include "GameFramework/GameUserSettings.h"
#include "GameSettings.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FInputConfigDelegate, const FLoadedMappableConfigContainer& Config);

/**
 * Bartender Ender Game Settings.
 */
UCLASS()
class UGameSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UGameSettings();

	static UGameSettings& Get();

	// Registers provided input config to make it available for the player.
	void RegisterInputConfig(ECommonInputType Type, const UPlayerMappableInputConfig* NewConfig, bool bIsActive);

	// Unregisters provided input config and returns number of removed configs.
	int32 UnregisterInputConfig(const UPlayerMappableInputConfig* Config);

	// Activates provided input config if it was registered.
	void ActivateInputConfig(const UPlayerMappableInputConfig* Config);

	// Deactivates provided input config.
	void DeactivateInputConfig(const UPlayerMappableInputConfig* Config);

	// Returns RegisteredInputConfigs array with all it's currently registered input configs.
	const auto& GetRegisteredInputConfigs() const { return RegisteredInputConfigs; }

private:
	// Returns config index if it's registered or INDEX_NONE otherwise.
	FORCEINLINE int32 GetRegisteredConfigIndex(const UPlayerMappableInputConfig* const Config) const;

public:
	// Delegate broadcasting when a new input config has been successfully registered.
	FInputConfigDelegate OnInputConfigRegistered;

	// Delegate broadcasting when a registered input config has been successfully activated.
	FInputConfigDelegate OnInputConfigActivated;

	// Delegate broadcasting when a registered input config has been successfully deactivated.
	FInputConfigDelegate OnInputConfigDeactivated;

private:
	// Currently registered input configs.
	UPROPERTY(VisibleAnywhere)
	TArray<FLoadedMappableConfigContainer> RegisteredInputConfigs;
};
