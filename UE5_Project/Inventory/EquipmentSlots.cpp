// Bartender Ender
// Naughty Panda @ 2022

#include "EquipmentSlots.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "BartenderEnder/LogCategories.h"
#include "BartenderEnder/Item/ItemInstance.h"
#include "BartenderEnder/Item/ItemSubsystem.h"
#include "BartenderEnder/Item/Equipment/EquipmentDefinition.h"
#include "BartenderEnder/Item/Equipment/EquipmentInstance.h"

UEquipmentSlots::UEquipmentSlots(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;

	Slots.Reserve(NumSlots);
}

void UEquipmentSlots::BeginPlay()
{
	checkf(EquipmentType != EEquipmentType::None, TEXT("UEquipmentSlots::BeginPlay: %s on %s has invalid Equipment Type!"), *GetName(), *GetNameSafe(GetOwner()));

	if (Slots.Num() < NumSlots)
	{
		Slots.AddDefaulted(NumSlots - Slots.Num());
	}

	Super::BeginPlay();
}

void UEquipmentSlots::PrintSlotInfo(int32 SlotIndex)
{
	if (!Slots.IsValidIndex(SlotIndex))
	{
		return;
	}

	const FString Message("Slot: " + FString::FromInt(SlotIndex) + " - " + GetNameSafe(Slots[SlotIndex]));
	GEngine->AddOnScreenDebugMessage(0, GetWorld()->GetDeltaSeconds(), FColor::Cyan, Message);
}

void UEquipmentSlots::SelectSlot(int32 NewSlotIndex, FGameplayTag ActionTag)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	FGameplayEventData EventData;
	EventData.EventMagnitude = Slots.IsValidIndex(NewSlotIndex) ? NewSlotIndex : 0.f;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, ActionTag, EventData);
}

void UEquipmentSlots::SelectNextActiveSlot()
{
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 CurrentIndex = ActiveSlotIndex == INDEX_NONE ? Slots.Num() - 1 : ActiveSlotIndex;
	int32 NewIndex = ActiveSlotIndex;

	do
	{
		NewIndex = ++NewIndex % Slots.Num();
		if (Slots[NewIndex])
		{
			SetActiveSlotIndex(NewIndex);
			return;
		}
	}
	while (NewIndex != CurrentIndex);
}

void UEquipmentSlots::SelectPreviousActiveSlot()
{
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 CurrentIndex = ActiveSlotIndex == INDEX_NONE ? Slots.Num() - 1 : ActiveSlotIndex;
	int32 NewIndex = ActiveSlotIndex;

	do
	{
		NewIndex = (--NewIndex + Slots.Num()) % Slots.Num();
		if (Slots[NewIndex])
		{
			SetActiveSlotIndex(NewIndex);
			return;
		}
	}
	while (NewIndex != CurrentIndex);
}

void UEquipmentSlots::SetActiveSlotIndex(int32 NewIndex)
{
	if (!Slots.IsValidIndex(NewIndex) || ActiveSlotIndex == NewIndex)
	{
		UE_LOG(LogInventory, Error, TEXT("SetActiveSlotIndex: Invalid slot index %i!"), NewIndex);
		return;
	}

	UnequipItemFromSlot(false);
	ActiveSlotIndex = NewIndex;
	EquipItemInSlot();
}

int32 UEquipmentSlots::GetNextEmptySlot() const
{
	return Slots.IndexOfByPredicate(
		[](const UItemInstance* Item)-> bool
		{
			return Item == nullptr;
		});
}

int32 UEquipmentSlots::GetItemSlot(UItemInstance* ItemToFind) const
{
	return Slots.IndexOfByPredicate([&ItemToFind](const auto& SlotItem) -> bool
	{
		if (!ItemToFind || !SlotItem)
		{
			return false;
		}

		return SlotItem->template FindFragment<UItemFragment_Equipment>()->EquipmentDefinition == ItemToFind->FindFragment<UItemFragment_Equipment>()->EquipmentDefinition;
	});
}

UItemInstance* UEquipmentSlots::GetActiveSlotItem() const
{
	return Slots.IsValidIndex(ActiveSlotIndex) ? Slots[ActiveSlotIndex] : nullptr;
}

AActor* UEquipmentSlots::GetActiveSlotItemActor() const
{
	if (!EquippedItem)
	{
		return nullptr;
	}

	const TArray<AActor*> SpawnedActors = EquippedItem->GetSpawnedActors();
	return SpawnedActors.IsEmpty() ? nullptr : SpawnedActors[0];
}

TArray<AActor*> UEquipmentSlots::GetActiveSlotItemActors() const
{
	return EquippedItem ? EquippedItem->GetSpawnedActors() : TArray<AActor*>();
}

void UEquipmentSlots::AddItemToSlot(UItemInstance* ItemInstance, int32 SlotIndex)
{
	if (!Slots.IsValidIndex(SlotIndex) || !ItemInstance)
	{
		UE_LOG(LogInventory, Error, TEXT("AddItemToSlot: Invalid slot index %i or ItemInstance %s!"), SlotIndex, *GetNameSafe(ItemInstance));
		return;
	}

	if (Slots[SlotIndex] == nullptr)
	{
		Slots[SlotIndex] = ItemInstance;
	}
}

UItemInstance* UEquipmentSlots::RemoveItemFromSlot(int32 SlotIndex)
{
	if (!Slots.IsValidIndex(SlotIndex))
	{
		UE_LOG(LogInventory, Error, TEXT("RemoveItemFromSlot: Invalid slot index: %i!"), SlotIndex);
		return nullptr;
	}

	if (ActiveSlotIndex == SlotIndex)
	{
		UnequipItemFromSlot(true);
		ActiveSlotIndex = INDEX_NONE;
	}

	UItemInstance* RemovedItem = Slots[SlotIndex];
	Slots[SlotIndex] = nullptr;

	return RemovedItem;
}

void UEquipmentSlots::SetEquipmentType(EEquipmentType NewType)
{
	if (EquipmentType == EEquipmentType::None)
	{
		EquipmentType = NewType;
	}
}

void UEquipmentSlots::EquipItemInSlot()
{
	if (!Slots.IsValidIndex(ActiveSlotIndex) || EquippedItem)
	{
		UE_LOG(LogInventory, Error, TEXT("EquipItemInSlot: Invalixd active slot inde: %i!"), ActiveSlotIndex);
		return;
	}

	UItemInstance* ActiveSlotItem = Slots[ActiveSlotIndex];
	if (!ActiveSlotItem)
	{
		UE_LOG(LogInventory, Error, TEXT("EquipItemInSlot: Invalid active slot Item!"));
		return;
	}

	const UItemFragment_Equipment* EquipmentFragment = ActiveSlotItem->FindFragment<UItemFragment_Equipment>();
	if (!EquipmentFragment)
	{
		UE_LOG(LogInventory, Error, TEXT("EquipItemInSlot: %s is not an equipment!"), *GetNameSafe(ActiveSlotItem));
		return;
	}

	const TSubclassOf<UEquipmentDefinition> EquipmentDefinition = EquipmentFragment->EquipmentDefinition;
	if (!EquipmentDefinition)
	{
		UE_LOG(LogInventory, Error, TEXT("EquipItemInSlot: No Equipment Definition for %s!"), *GetNameSafe(ActiveSlotItem));
		return;
	}

	const UItemSubsystem* ItemSubsystem = UItemSubsystem::FindItemSubsystem(GetWorld());
	if (!ItemSubsystem)
	{
		UE_LOG(LogInventory, Error, TEXT("EquipItemInSlot: Invalid Item Subsystem!"));
		return;
	}


	// Create new item or use the existing one.
	EquippedItem = ItemSubsystem->EquipItemOnPlayer(EquipmentDefinition);

	if (EquippedItem)
	{
		EquippedItem->SetInstigator(ActiveSlotItem);
	}
}

void UEquipmentSlots::UnequipItemFromSlot(bool bDestroyItem)
{
	if (!EquippedItem)
	{
		UE_LOG(LogInventory, Error, TEXT("UnequipItemFromSlot: Tried to unequip NULL item!"));
		return;
	}

	const UItemSubsystem* ItemSubsystem = UItemSubsystem::FindItemSubsystem(GetWorld());
	if (!ItemSubsystem)
	{
		UE_LOG(LogInventory, Error, TEXT("UnequipItemFromSlot: Invalid Item Subsystem!"));
		return;
	}

	// TODO: delete item or just disable it.

	ItemSubsystem->UnequipItemFromPlayer(EquippedItem, bDestroyItem);
	EquippedItem = nullptr;
}
