// Bartender Ender
// Naughty Panda @ 2022

#include "InputConfig.h"

UInputConfig::UInputConfig(const FObjectInitializer& ObjectInitializer)
{
}

const UInputAction* UInputConfig::FindNativeInputActionForTag(const FGameplayTag& ForTag) const
{
	for (const auto& [InputAction, InputTag] : NativeInputActions)
	{
		if (InputAction && InputTag == ForTag)
		{
			return InputAction;
		}
	}

	UE_LOG(LogInput, Error, TEXT("No NativeInputAction for InputTag %s!"), *ForTag.ToString());
	return nullptr;
}

const UInputAction* UInputConfig::FindAbilityInputActionForTag(const FGameplayTag& ForTag) const
{
	for (const auto& [InputAction, InputTag] : AbilityInputActions)
	{
		if (InputAction && InputTag == ForTag)
		{
			return InputAction;
		}
	}

	UE_LOG(LogInput, Error, TEXT("No AbilityInputAction for InputTag %s!"), *ForTag.ToString());
	return nullptr;
}
