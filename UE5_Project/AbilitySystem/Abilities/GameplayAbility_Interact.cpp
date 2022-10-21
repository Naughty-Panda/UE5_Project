// Bartender Ender
// Naughty Panda @ 2022

#include "GameplayAbility_Interact.h"
#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"
#include "BartenderEnder/AbilitySystem/Tasks/AbilityTask_GrantNearbyInteraction.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(Tag_Ability_Interaction_Activate, "Ability.Interaction.Activate");

UGameplayAbility_Interact::UGameplayAbility_Interact(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EGameplayAbilityActivationPolicy::OnSpawn;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGameplayAbility_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (const UAbilitySystemComponent* AbilitySystem = UGameplayAbility::GetAbilitySystemComponentFromActorInfo(); !AbilitySystem)
	{
		return;
	}

	UAbilityTask_GrantNearbyInteraction* AbilityTask = UAbilityTask_GrantNearbyInteraction::GrantAbilitiesForNearbyInteractors(this, InteractionScanRate, InteractionScanRange);
	AbilityTask->ReadyForActivation();
}

void UGameplayAbility_Interact::UpdateInteractions(const TArray<FInteractionOption>& InteractionOptions)
{
	// TODO: this
	CurrentInteractionOptions = InteractionOptions;
}

void UGameplayAbility_Interact::TriggerInteraction()
{
	if (CurrentInteractionOptions.Num() == 0)
	{
		return;
	}

	if (const UAbilitySystemComponent* AbilitySystem = UGameplayAbility::GetAbilitySystemComponentFromActorInfo(); !AbilitySystem)
	{
		return;
	}

	const FInteractionOption& InteractionOption = CurrentInteractionOptions[0];

	const AActor* Instigator = GetAvatarActorFromActorInfo();
	AActor* InteractableTargetActor = UInteractableStatics::GetActorFromInteractableTarget(InteractionOption.InteractableTarget);

	FGameplayEventData EventData;
	EventData.EventTag = Tag_Ability_Interaction_Activate;
	EventData.Instigator = Instigator;
	EventData.Target = InteractableTargetActor;

	// Allow interactable target to customize EventData if needed.
	InteractionOption.InteractableTarget->CustomizeInteractionEventData(Tag_Ability_Interaction_Activate, EventData);

	AActor* TargetActor = const_cast<AActor*>(EventData.Target);

	FGameplayAbilityActorInfo ActorInfo;
	ActorInfo.InitFromActor(InteractableTargetActor, TargetActor, InteractionOption.TargetAbilitySystem);

	InteractionOption.TargetAbilitySystem->TriggerAbilityFromGameplayEvent(
		InteractionOption.TargetInteractionAbilityHandle,
		&ActorInfo,
		Tag_Ability_Interaction_Activate,
		&EventData,
		*InteractionOption.TargetAbilitySystem
	);
}
