// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "BaseHealingExecution.generated.h"

/**
 * Execution calculation used by gameplay effects to apply healing.
 */
UCLASS()
class UBaseHealingExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UBaseHealingExecution();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
