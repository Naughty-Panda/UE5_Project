// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryManager.generated.h"

class UItemInstance;
class UItemDefinition;

/**
 * Entry of an item stored in Inventory List.
 */
USTRUCT(BlueprintType)
struct FInventoryEntry
{
	GENERATED_BODY()

private:
	friend struct FInventoryList;
	friend class UInventoryManager;

private:
	// Current Item Instance.
	UPROPERTY()
	TObjectPtr<UItemInstance> ItemInstance;

	// Stack Count for this item.
	UPROPERTY()
	int32 StackCount = 0;
};

/**
 * List of stored items.
 */
USTRUCT(BlueprintType)
struct FInventoryList
{
	GENERATED_BODY()

public:
	FInventoryList() : OwnerComponent(nullptr)
	{
	}

	FInventoryList(TObjectPtr<UActorComponent> InOwnerComponent) : OwnerComponent(InOwnerComponent)
	{
	}

public:
	UE_NODISCARD TArray<UItemInstance*> GetAllItems() const;
	UE_NODISCARD UItemInstance* CreateEntry(TSubclassOf<UItemDefinition> ItemDefinition, int32 StackCount);
	void AddEntry(TObjectPtr<UItemInstance> ItemInstance, int32 StackCount);
	void RemoveEntry(TObjectPtr<UItemInstance> ItemInstance);

private:
	// Pointer to owner component.
	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;

	// List of inventory entries.
	UPROPERTY()
	TArray<FInventoryEntry> Entries;
};

/**
 * Inventory Manager Component.
 */
UCLASS(BlueprintType, Meta=(BlueprintSpawnableComponent))
class UInventoryManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
	FInventoryList InventoryList;
};
