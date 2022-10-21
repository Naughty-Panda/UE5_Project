// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInput/Public/InputModifiers.h"
#include "GameInputModifiers.generated.h"

/**
 * Enhanced Input Custom Modifiers for Bartender Ender.
 */

/**
 * Scales input based on Delta Time and optional additional multiplier.
 */
UCLASS(NotBlueprintable, MinimalAPI, Meta = (DisplayName = "Delta Time Scaled"))
class UInputModifierDeltaTimeScaled : public UInputModifier
{
	GENERATED_BODY()

public:
	// Additional axis-based multiplier.
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = Settings)
	FVector Multiplier = FVector::OneVector;

protected:
	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime) override;
};
