// Bartender Ender
// Naughty Panda @ 2022

#include "IInteractable.h"

AActor* UInteractableStatics::GetActorFromInteractableTarget(TScriptInterface<IInteractable> InteractableTarget)
{
	UObject* InteractableObject = InteractableTarget.GetObject();
	if (!InteractableObject)
	{
		return nullptr;
	}

	if (AActor* InteractableActor = Cast<AActor>(InteractableObject))
	{
		return InteractableActor;
	}

	if (UActorComponent* InteractableComponent = Cast<UActorComponent>(InteractableObject))
	{
		return InteractableComponent->GetOwner();
	}

	return nullptr;
}

void UInteractableStatics::GetInteractableTargetsFromActor(AActor* Actor, TArray<TScriptInterface<IInteractable>>& OutInteractableTargetss)
{
	if (TScriptInterface<IInteractable> InteractableActor(Actor); InteractableActor)
	{
		OutInteractableTargetss.Add(InteractableActor);
	}

	TArray<UActorComponent*> InteractableComponents = Actor ? Actor->GetComponentsByInterface(UInteractable::StaticClass()) : TArray<UActorComponent*>();
	for (UActorComponent* InteractableComponent : InteractableComponents)
	{
		OutInteractableTargetss.Add(TScriptInterface<IInteractable>(InteractableComponent));
	}
}

void UInteractableStatics::AppendInteractableTargetsFromOverlapResults(const TArray<FOverlapResult>& OverlapResults, TArray<TScriptInterface<IInteractable>>& OutInteractableTargets)
{
	for (const auto& Overlap : OverlapResults)
	{
		AddUniqueInteractableTargetsFrom(Overlap, OutInteractableTargets);
	}
}

void UInteractableStatics::AppendInteractableTargetsFromHitResult(const FHitResult& HitResult, TArray<TScriptInterface<IInteractable>>& OutInteractableTargets)
{
	AddUniqueInteractableTargetsFrom(HitResult, OutInteractableTargets);
}
