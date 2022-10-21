// Bartender Ender
// Naughty Panda @ 2022

#include "BaseHealingExecution.h"
#include "BartenderEnder/AbilitySystem/Attributes/CombatSet.h"
#include "BartenderEnder/AbilitySystem/Attributes/HealthSet.h"

struct FHealingStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseHealingCaptureDefinition;

	FHealingStatics()
	{
		BaseHealingCaptureDefinition = FGameplayEffectAttributeCaptureDefinition(
			UCombatSet::GetBaseHealingAttribute(),
			EGameplayEffectAttributeCaptureSource::Source,
			true);
	}
};

static FHealingStatics& HealingStatics()
{
	static FHealingStatics Statics;
	return Statics;
}

UBaseHealingExecution::UBaseHealingExecution()
{
	RelevantAttributesToCapture.Add(HealingStatics().BaseHealingCaptureDefinition);
}

void UBaseHealingExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const float HealingDone = 5.f;

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UHealthSet::GetHealthAttribute(), EGameplayModOp::Additive, HealingDone));
}
