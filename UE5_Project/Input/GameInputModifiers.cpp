// Bartender Ender
// Naughty Panda @ 2022

#include "GameInputModifiers.h"

FInputActionValue UInputModifierDeltaTimeScaled::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	return CurrentValue.Get<FVector>() * Multiplier * DeltaTime;
}
