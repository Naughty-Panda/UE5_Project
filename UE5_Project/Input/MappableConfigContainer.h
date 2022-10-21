// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "CommonInputBaseTypes.h"
#include "GameplayTagContainer.h"
#include "PlayerMappableInputConfig.h"
#include "MappableConfigContainer.generated.h"

/**
 * Container that maps player input configs with CommonUI input type.
 */
USTRUCT(BlueprintType)
struct FLoadedMappableConfigContainer
{
	GENERATED_BODY()

public:
	FLoadedMappableConfigContainer() = default;

	FLoadedMappableConfigContainer(const UPlayerMappableInputConfig* InInputConfig, const ECommonInputType InInputType, const bool IsActive)
		: InputConfig(InInputConfig), InputType(InInputType), bIsActive(IsActive)
	{
	}

	// Player mappable input config to be registered with Enhanced Input Subsystem.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	const UPlayerMappableInputConfig* InputConfig = nullptr;

	// Type of player device this config should be applied to.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ECommonInputType InputType = ECommonInputType::Count;

	// If this config is currently active.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsActive = false;
};

/**
 * Container that maps unloaded player input configs with CommonUI input type.
 */
USTRUCT()
struct FMappableConfigContainer
{
	GENERATED_BODY()

public:
	FMappableConfigContainer() = default;

	bool CanBeActivated() const;

	// Tries to register provided container with the GameSettings.
	static bool RegisterContainer(const FMappableConfigContainer& Container);
	// Tries to register provided container and then activates it.
	static bool ActivateContainer(const FMappableConfigContainer& Container);
	static void DeactivateContainer(const FMappableConfigContainer& Container);
	static bool UnregisterContainer(const FMappableConfigContainer& Container);


	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UPlayerMappableInputConfig> InputConfig;

	// Player device input type for this config.
	UPROPERTY(EditAnywhere)
	ECommonInputType InputType = ECommonInputType::Count;

	// Array of platform-specific tags required for this config to be activated.
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer DependentPlatformTags;

	// Array of platform-specific tags preventing this config from being activated.
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer ExcludedPlatformTags;

	UPROPERTY(EditAnywhere)
	bool bShouldActivateAutomatically = true;
};
