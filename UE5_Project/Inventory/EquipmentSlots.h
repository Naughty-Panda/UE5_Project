// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BartenderEnder/CoreTypes.h"
#include "EquipmentSlots.generated.h"

class UItemInstance;
class UEquipmentInstance;
struct FGameplayTag;

/**
 * Selectable Equipment Slots Component.
 */
UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class UEquipmentSlots : public UActorComponent
{
	GENERATED_BODY()

public:
	UEquipmentSlots(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void PrintSlotInfo(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SelectSlot(int32 NewSlotIndex, FGameplayTag ActionTag);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SelectNextActiveSlot();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SelectPreviousActiveSlot();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetActiveSlotIndex(int32 NewIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Inventory")
	TArray<UItemInstance*> GetSlots() const { return Slots; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Inventory")
	int32 GetNextEmptySlot() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Inventory")
	int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Inventory")
	int32 GetItemSlot(UItemInstance* ItemToFind) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Inventory")
	UItemInstance* GetActiveSlotItem() const;

	// Returns the first spawned actor for currently equipped item.
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	AActor* GetActiveSlotItemActor() const;

	// Returns an array of spawned actors for currently equipped item.
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<AActor*> GetActiveSlotItemActors() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddItemToSlot(UItemInstance* ItemInstance, int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UItemInstance* RemoveItemFromSlot(int32 SlotIndex);

	// Returns Equipment Type for current inventory.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	EEquipmentType GetEquipmentType() const { return EquipmentType; }

	// Assigns new Equipment Type for current inventory.
	void SetEquipmentType(EEquipmentType NewType);

private:
	void EquipItemInSlot();
	void UnequipItemFromSlot(bool bDestroyItem);
	// TODO: Separate func to destroy item?

protected:
	// Default equipment type for this inventory.
	EEquipmentType EquipmentType = EEquipmentType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	int32 NumSlots = 5;

private:
	// Array of inventory slots.
	TArray<TObjectPtr<UItemInstance>> Slots;

	// Currently equipped Item.
	UPROPERTY()
	TObjectPtr<UEquipmentInstance> EquippedItem;

	// Current active slot index.
	int32 ActiveSlotIndex = INDEX_NONE;
};
