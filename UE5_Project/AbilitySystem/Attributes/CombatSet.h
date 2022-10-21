// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "BaseAttributeSet.h"
#include "CombatSet.generated.h"

/**
 * Attribute Set used in combat to apply damage or healing.
 */
UCLASS(BlueprintType)
class UCombatSet : public UBaseAttributeSet
{
	GENERATED_BODY()

public:
	UCombatSet();

	ATTRIBUTE_ACCESSORS(UCombatSet, BaseDamage);
	ATTRIBUTE_ACCESSORS(UCombatSet, BaseHealing);

private:
	// Base Damage to be used in damage execution calculation. Applies damage to the target.
	UPROPERTY(BlueprintReadOnly, Category = "Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseDamage;

	// Base Healing to be used in healing execution calculation. Applies healing to the target.
	UPROPERTY(BlueprintReadOnly, Category = "Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseHealing;
};
