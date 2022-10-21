// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "IPickupable.generated.h"

class UGameplayAbility;
class UItemInstance;
class UItemDefinition;
class UInventoryManager;

/**
 * Contains information to spawn a required Item.
 */
USTRUCT(BlueprintType)
struct FPickupTemplate
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UItemDefinition> ItemDefinition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 StackCount = 1;
};

/**
 * Contains Items and Item Templates that can be picked up from an Actor.
 */
USTRUCT(BlueprintType)
struct FPickupInventory
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup Inventory")
	TArray<TObjectPtr<UItemInstance>> Items;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup Inventory")
	TArray<FPickupTemplate> ItemTemplates;
};



/**
 * UE Interface for things that can be picked up by someone.
 */
UINTERFACE(MinimalAPI, BlueprintType, Meta = (CannotImplementInterfaceInBlueprint))
class UPickupable : public UInterface
{
	GENERATED_BODY()
};

/**
 * C++ Interface for things that can be picked up by someone.
 */
class IPickupable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual const FPickupInventory& GetPickupInventory() const = 0;
};



/**
 * Blueprint function library to interact with IPickupable interface.
 */
UCLASS()
class UPickupableStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Inventory", Meta = (WorldContext = "Ability"))
	static TScriptInterface<IPickupable> GetIPickupableFromActorInfo(UGameplayAbility* Ability);

	UFUNCTION(BlueprintCallable, Category = "Inventory", Meta = (WorldContext = "Ability"))
	static void AddPickupInventory(AActor* ReceivingActor, TScriptInterface<IPickupable> PickupableActor);
};
