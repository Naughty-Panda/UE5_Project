// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "BaseDamageExecution.generated.h"

/**
 * Execution calculation used by gameplay effects to apply damage.
 */
UCLASS()
class UBaseDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UBaseDamageExecution();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
