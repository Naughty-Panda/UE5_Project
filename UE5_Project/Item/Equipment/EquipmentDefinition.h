// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EquipmentDefinition.generated.h"

class UEquipmentInstance;
class UAbilitySet;

/**
 * Equipment Actor Spawn Info.
 */
USTRUCT()
struct FEquipmentActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Equipment")
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	FName AttachmentSocket;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	FTransform AttachmentTransform;
};

/**
 * Definition of an item that can be equipped.
 */
UCLASS(Blueprintable, BlueprintType, Const, Abstract)
class UEquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:
	UEquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	// Instance subclass to be used with this equipment.
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TSubclassOf<UEquipmentInstance> EquipmentInstance;

	// Ability Sets attached to this equipment.
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TArray<TObjectPtr<const UAbilitySet>> AbilitySets;

	// Array of Equipment actors to be spawned.
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TArray<FEquipmentActor> EquipmentToSpawn;
};
