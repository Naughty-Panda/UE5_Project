// Bartender Ender
// Naughty Panda @ 2022

#include "AbilityTask_GrantNearbyInteraction.h"
#include "AbilitySystemComponent.h"
#include "BartenderEnder/CoreTypes.h"
#include "BartenderEnder/Interface/IInteractable.h"

void UAbilityTask_GrantNearbyInteraction::Activate()
{
	SetWaitingOnAvatar();

	const UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("AbilityTask::Activate: No UWorld!"));
		return;
	}

	World->GetTimerManager().SetTimer(QueryTimerHandle, this, &UAbilityTask_GrantNearbyInteraction::QueryInteractables, InteractionScanRate, true);
}

UAbilityTask_GrantNearbyInteraction* UAbilityTask_GrantNearbyInteraction::GrantAbilitiesForNearbyInteractors(UGameplayAbility* OwningAbility, float InteractionScanRate, float InteractionScanRange)
{
	UAbilityTask_GrantNearbyInteraction* AbilityTask = NewAbilityTask<UAbilityTask_GrantNearbyInteraction>(OwningAbility);
	AbilityTask->InteractionScanRate = InteractionScanRate;
	AbilityTask->InteractionScanRange = InteractionScanRange;
	return AbilityTask;
}

void UAbilityTask_GrantNearbyInteraction::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);

	const UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("AbilityTask::OnDestroy: No UWorld!"));
		return;
	}

	World->GetTimerManager().ClearTimer(QueryTimerHandle);
}

void UAbilityTask_GrantNearbyInteraction::QueryInteractables()
{
	const UWorld* World = GetWorld();
	AActor* OwnerActor = GetAvatarActor();

	if (!World || !OwnerActor)
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("AbilityTask::QueryInteractables: Invalid UWorld or Owner!"));
		return;
	}

	TArray<FOverlapResult> OverlapResults;
	const FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(UAbilityTask_GrantNearbyInteraction), false);
	const FVector TraceStartLocation = OwnerActor->GetActorLocation();
	const FCollisionShape TraceShape = FCollisionShape::MakeSphere(InteractionScanRange);
	World->OverlapMultiByChannel(OUT OverlapResults, TraceStartLocation, FQuat::Identity, TraceChannel_Interaction, TraceShape, QueryParams);

	if (OverlapResults.Num() == 0)
	{
		return;
	}

	TArray<TScriptInterface<IInteractable>> InteractableTargets;
	UInteractableStatics::AppendInteractableTargetsFromOverlapResults(OverlapResults, InteractableTargets);

	FInteractionQuery InteractionQuery;
	InteractionQuery.RequestingActor = OwnerActor;
	InteractionQuery.RequestingController = Cast<AController>(OwnerActor->GetOwner());

	TArray<FInteractionOption> InteractionOptions;
	for (const auto& InteractableTarget : InteractableTargets)
	{
		FInteractionOptionBuilder InteractionOptionBuilder(InteractableTarget, InteractionOptions);
		InteractableTarget->CollectInteractionOptions(InteractionQuery, InteractionOptionBuilder);
	}

	// Check if options have any abilities to grant to the character.
	for (const auto& InteractionOption : InteractionOptions)
	{
		if (InteractionOption.InteractionAbilityToGrant)
		{
			FObjectKey AbilityKey(InteractionOption.InteractionAbilityToGrant);

			if (!InteractionAbilityCache.Find(AbilityKey))
			{
				const FGameplayAbilitySpec AbilitySpec(InteractionOption.InteractionAbilityToGrant, 1, INDEX_NONE, this);
				const FGameplayAbilitySpecHandle AbilitySpecHandle = AbilitySystemComponent->GiveAbility(AbilitySpec);
				InteractionAbilityCache.Add(AbilityKey, AbilitySpecHandle);
			}
		}
	}
}
