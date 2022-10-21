// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "InputConfig.h"
#include "PlayerInputComponent.generated.h"

class UInputConfig;
class UEnhancedInputLocalPlayerSubsystem;
struct FLoadedMappableConfigContainer;

/**
 * Player Input Component manages input mappings and bindings for the player.
 */
UCLASS(Config = Input)
class UPlayerInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	void AddInputMappings(const UInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	void RemoveInputMappings(const UInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;

	void RemoveBinds(TArray<uint32>& BindHandles);

	void AddInputConfig(const FLoadedMappableConfigContainer& ConfigContainer, UEnhancedInputLocalPlayerSubsystem* InputSubsystem);
	void RemoveInputConfig(const FLoadedMappableConfigContainer& ConfigContainer, UEnhancedInputLocalPlayerSubsystem* InputSubsystem);

	template <class UserClass, typename FuncType>
	void BindNativeAction(const UInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func)
	{
		check(InputConfig);

		if (const UInputAction* InputAction = InputConfig->FindNativeInputActionForTag(InputTag))
		{
			BindAction(InputAction, TriggerEvent, Object, Func);
		}
	}

	template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
	{
		check(InputConfig);

		for (const auto& [InputAction, InputTag] : InputConfig->AbilityInputActions)
		{
			if (InputAction && InputTag.IsValid())
			{
				if (PressedFunc)
				{
					BindHandles.Add(BindAction(InputAction, ETriggerEvent::Triggered, Object, PressedFunc, InputTag).GetHandle());
				}

				if (ReleasedFunc)
				{
					BindHandles.Add(BindAction(InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, InputTag).GetHandle());
				}
			}
		}
	}
};
