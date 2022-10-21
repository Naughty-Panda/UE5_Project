// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputConfig.generated.h"

class UInputAction;

/**
 * Link between Input Action and it's Gameplay Tag.
 */
USTRUCT(BlueprintType)
struct FTaggedInputAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	const UInputAction* InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

/**
 * Input Configuration for Bartender Ender.
 */
UCLASS(BlueprintType, Const)
class UInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UInputConfig(const FObjectInitializer& ObjectInitializer);

	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& ForTag) const;
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& ForTag) const;

public:
	// Array of Native input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FTaggedInputAction> NativeInputActions;

	// Array of Ability input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FTaggedInputAction> AbilityInputActions;
};
