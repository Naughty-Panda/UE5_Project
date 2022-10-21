// Bartender Ender
// Naughty Panda @ 2022

#include "InventoryManager.h"
#include "BartenderEnder/LogCategories.h"
#include "BartenderEnder/Item/ItemDefinition.h"
#include "BartenderEnder/Item/ItemInstance.h"
#include "BartenderEnder/Item/ItemSubsystem.h"

TArray<UItemInstance*> FInventoryList::GetAllItems() const
{
	TArray<UItemInstance*> AllItems;
	AllItems.Reserve(Entries.Num());

	for (const FInventoryEntry& Entry : Entries)
	{
		if (Entry.ItemInstance)
		{
			AllItems.Add(Entry.ItemInstance);
		}
	}

	return AllItems;
}

UItemInstance* FInventoryList::CreateEntry(TSubclassOf<UItemDefinition> ItemDefinition, int32 StackCount)
{
	check(ItemDefinition);
	check(OwnerComponent);

	const UItemSubsystem* ItemSubsystem = UItemSubsystem::FindItemSubsystem(OwnerComponent->GetWorld());
	if (!ItemSubsystem)
	{
		UE_LOG(LogItem, Error, TEXT("AddEntry: Invalid Item Subsystem!"));
		return nullptr;
	}

	FInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.ItemInstance = ItemSubsystem->CreateItemInstance(OwnerComponent->GetOwner());
	NewEntry.ItemInstance->SetItemDefinition(ItemDefinition);
	NewEntry.StackCount = StackCount;

	for (TObjectPtr<UItemFragment> ItemFragment : GetDefault<UItemDefinition>(ItemDefinition)->Fragments)
	{
		if (ItemFragment)
		{
			ItemFragment->OnItemCreated(NewEntry.ItemInstance);
		}
	}

	return NewEntry.ItemInstance;
}

void FInventoryList::RemoveEntry(TObjectPtr<UItemInstance> ItemInstance)
{
	check(ItemInstance);

	for (auto EntryIterator = Entries.CreateIterator(); EntryIterator; ++EntryIterator)
	{
		if (FInventoryEntry& Entry = *EntryIterator; Entry.ItemInstance == ItemInstance)
		{
			// TODO: Check if it is equipment and destroy its actors?
			EntryIterator.RemoveCurrent();
		}
	}
}

UInventoryManager::UInventoryManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), InventoryList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
}
