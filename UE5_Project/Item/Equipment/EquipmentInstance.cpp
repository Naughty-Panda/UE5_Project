// Bartender Ender
// Naughty Panda @ 2022

#include "EquipmentInstance.h"
#include "EquipmentDefinition.h"
#include "Algo/ForEach.h"
#include "BartenderEnder/Item/ItemSubsystem.h"
#include "GameFramework/Character.h"
#include "BartenderEnder/LogCategories.h"

UEquipmentInstance::UEquipmentInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UWorld* UEquipmentInstance::GetWorld() const
{
	const APawn* Owner = GetPawn();
	return Owner ? Owner->GetWorld() : nullptr;
}

UItemSubsystem* UEquipmentInstance::GetItemSubsystem() const
{
	// TODO: Remove this.
	check(GEngine);

	const ULocalPlayer* LocalPlayer = GEngine->GetFirstGamePlayer(GetWorld());
	if (!LocalPlayer)
	{
		UE_LOG(LogItem, Error, TEXT("GetItemSubsystem: Invalid Local Player!"));
		return nullptr;
	}

	return LocalPlayer->GetSubsystem<UItemSubsystem>();
}

APawn* UEquipmentInstance::GetPawnTyped(TSubclassOf<APawn> PawnType) const
{
	if (UClass* RequestedPawnType = PawnType)
	{
		if (GetOuter()->IsA(RequestedPawnType))
		{
			return Cast<APawn>(GetOuter());
		}
	}

	return nullptr;
}

void UEquipmentInstance::SpawnEquipmentActors(const TArray<FEquipmentActor>& ActorsToSpawn)
{
	if (APawn* Owner = GetPawn())
	{
		const ACharacter* Character = Cast<ACharacter>(Owner);
		USceneComponent* AttachTarget = Character ? Character->GetMesh() : Owner->GetRootComponent();

		const UItemSubsystem* ItemSubsystem = UItemSubsystem::FindItemSubsystem(GetWorld());
		if (!ItemSubsystem)
		{
			UE_LOG(LogItem, Error, TEXT("SpawnEquipmentActors: Invalid Item Subsystem!"));
			return;
		}

		for (const FEquipmentActor& Actor : ActorsToSpawn)
		{
			// Request ItemSubsystem to spawn a new actor for us.
			AActor* NewActor = ItemSubsystem->SpawnEquipmentActor(Actor.ActorToSpawn, Owner);

			if (!NewActor)
			{
				UE_LOG(LogItem, Error, TEXT("SpawnEquipmentActors: Invalid Item Subsystem!"));
				break;
			}

			NewActor->SetActorRelativeTransform(Actor.AttachmentTransform);
			NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, Actor.AttachmentSocket);
			SpawnedActors.Add(NewActor);
		}
	}
}

void UEquipmentInstance::DestroyEquipmentActors()
{
	UItemSubsystem* ItemSubsystem = UItemSubsystem::FindItemSubsystem(GetWorld());
	if (!ItemSubsystem)
	{
		UE_LOG(LogItem, Error, TEXT("DestroyEquipmentActors: Invalid Item Subsystem!"));
		return;
	}

	for (const TObjectPtr<AActor> SpawnedActor : SpawnedActors)
	{
		ItemSubsystem->DestroyEquipmentActor(SpawnedActor);
	}
}

void UEquipmentInstance::SetEquipmentActorsHiddenInGame(bool bNewHidden)
{
	Algo::ForEach(SpawnedActors, [bNewHidden](const TObjectPtr<AActor> Actor)
	{
		Actor->SetActorHiddenInGame(bNewHidden);
	});
}
