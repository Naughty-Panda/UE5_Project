// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "BartenderEnder/Interface/IInteractable.h"
#include "BartenderEnder/Interface/IPickupable.h"
#include "GameFramework/Actor.h"
#include "ItemActor.generated.h"

class UItemDefinition;
class UItemInstance;

/**
 * Placeable Item Actor template.
 */
UCLASS(BlueprintType, Blueprintable)
class AItemActor : public AActor, public IInteractable, public IPickupable
{
	GENERATED_BODY()

public:
	AItemActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void CollectInteractionOptions(const FInteractionQuery& InteractionQuery, FInteractionOptionBuilder& OptionBuilder) override;
	virtual const FPickupInventory& GetPickupInventory() const override { return PickupInventory; }

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Item")
	UItemInstance* SpawnItem();

protected:
	// Update ItemActor's static mesh to look like an item it spawns.
	void UpdateItemMesh();

private:
	// Item Mesh.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", Meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> ItemMesh;

	// Item Definition for this item.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", Meta = (AllowPrivateAccess = true))
	TSubclassOf<UItemDefinition> ItemDefinition;

	// Option for interaction.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", Meta = (AllowPrivateAccess = true))
	FInteractionOption InteractionOption;

	// Inventory this item provides for pickup.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", Meta = (AllowPrivateAccess = true))
	FPickupInventory PickupInventory;

	// An instance of this item.
	UPROPERTY()
	TObjectPtr<UItemInstance> ItemInstance;
};
