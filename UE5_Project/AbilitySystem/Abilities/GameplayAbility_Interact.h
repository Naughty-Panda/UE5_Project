// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilityV2.h"
#include "BartenderEnder/Interface/IInteractable.h"
#include "GameplayAbility_Interact.generated.h"

/**
 * Gameplay Ability for interactions between IInteractables.
 */
UCLASS(Abstract)
class UGameplayAbility_Interact : public UGameplayAbilityV2
{
	GENERATED_BODY()

public:
	UGameplayAbility_Interact(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void UpdateInteractions(const TArray<FInteractionOption>& InteractionOptions);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TriggerInteraction();

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	TArray<FInteractionOption> CurrentInteractionOptions;

	// TODO: IndicatorDescriptor.
	UPROPERTY()
	TArray<TObjectPtr<UObject>> Indicators;

protected:
	// Default Interaction UserWidget class.
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TSoftClassPtr<UUserWidget> DefaultInteractionWidgetClass;

	// Interaction Scan Rate.
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionScanRate = .1f;

	// Interaction Scan Range.
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionScanRange = 500.f;
};
