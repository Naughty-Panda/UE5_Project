// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_GrantNearbyInteraction.generated.h"

/**
 * Ability Task to query nearby interactions.
 */
UCLASS()
class UAbilityTask_GrantNearbyInteraction : public UAbilityTask
{
	GENERATED_BODY()

public:
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", Meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = true))
	static UAbilityTask_GrantNearbyInteraction* GrantAbilitiesForNearbyInteractors(UGameplayAbility* OwningAbility, float InteractionScanRate, float InteractionScanRange);

private:
	virtual void OnDestroy(bool bInOwnerFinished) override;

	void QueryInteractables();

private:
	FTimerHandle QueryTimerHandle;

	float InteractionScanRate = .1f;
	float InteractionScanRange = 500.f;

	TMap<FObjectKey, FGameplayAbilitySpecHandle> InteractionAbilityCache;
};
