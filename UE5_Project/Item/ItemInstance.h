// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "ItemDefinition.h"
#include "UObject/Object.h"
#include "ItemInstance.generated.h"

class UItemFragment;

/**
 * Basic Item Instance.
 */
UCLASS(BlueprintType)
class UItemInstance : public UObject
{
	GENERATED_BODY()

private:
	friend class AItemActor;
	friend struct FInventoryList;

public:
	UItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	// Returns Item Definition.
	TSubclassOf<UItemDefinition> GetItemDefinition() const { return ItemDefinition; }

	// Returns true if item is an equipment
	UFUNCTION(BlueprintPure, Category = "Item")
	bool IsEquipment() const;

	// Gives this item to an Actor.
	UFUNCTION(BlueprintCallable, Category = "Item")
	bool GiveTo(AActor* ToActor);

	// Gives this item to the Player Character.
	UFUNCTION(BlueprintCallable, Category = "Item")
	bool GiveToPlayer();

	// Returns Item Fragment of specified subclass.
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Meta = (DeterminesOutputType = FragmentClass))
	const UItemFragment* FindFragmentByClass(TSubclassOf<UItemFragment> FragmentClass) const;

	// Returns Item Fragment of specified subclass.
	template <typename T>
	const T* FindFragment() const;

private:
	void SetItemDefinition(TSubclassOf<UItemDefinition> NewItemDefinition) { ItemDefinition = NewItemDefinition; }

private:
	UPROPERTY()
	TSubclassOf<UItemDefinition> ItemDefinition;

	// TODO: container with stat tags.
};

template <typename T>
const T* UItemInstance::FindFragment() const
{
	if (ItemDefinition)
	{
		return GetDefault<UItemDefinition>(ItemDefinition)->FindFragment<T>();
	}

	return nullptr;
}
