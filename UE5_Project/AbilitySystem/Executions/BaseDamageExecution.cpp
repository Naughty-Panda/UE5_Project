// Bartender Ender
// Naughty Panda @ 2022

#include "BaseDamageExecution.h"
#include "BartenderEnder/AbilitySystem/Attributes/CombatSet.h"
#include "BartenderEnder/AbilitySystem/Attributes/HealthSet.h"

struct FDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition HealthCaptureDefinition;
	FGameplayEffectAttributeCaptureDefinition BaseDamageCaptureDefinition;

	FDamageStatics()
	{
		HealthCaptureDefinition = FGameplayEffectAttributeCaptureDefinition(
			UHealthSet::GetHealthAttribute(),
			EGameplayEffectAttributeCaptureSource::Target,
			false);
		BaseDamageCaptureDefinition = FGameplayEffectAttributeCaptureDefinition(
			UCombatSet::GetBaseDamageAttribute(),
			EGameplayEffectAttributeCaptureSource::Source,
			true);
	}
};

static FDamageStatics& DamageStatics()
{
	static FDamageStatics DamageStatics;
	return DamageStatics;
}

UBaseDamageExecution::UBaseDamageExecution()
{
	RelevantAttributesToCapture.Add(DamageStatics().HealthCaptureDefinition);
	RelevantAttributesToCapture.Add(DamageStatics().BaseDamageCaptureDefinition);

#if WITH_EDITORONLY_DATA
	//TODO: Hide Health Definition?
	//InvalidScopedModifierAttributes.Add(DamageStatics().HealthCaptureDefinition);
#endif
}

void UBaseDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// TODO: proper damage execution.

	const float DamageDone = 10.f;

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UHealthSet::GetHealthAttribute(), EGameplayModOp::Additive, -DamageDone));
}
