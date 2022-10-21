// Bartender Ender
// Naughty Panda @ 2022

#include "EquipmentManager.h"
#include "AbilitySystemGlobals.h"
#include "EquipmentDefinition.h"
#include "EquipmentInstance.h"
#include "BartenderEnder/LogCategories.h"
#include "BartenderEnder/AbilitySystem/AbilitySystemComponentV2.h"
#include "BartenderEnder/Item/ItemSubsystem.h"

void FEquipmentEntry::ActivateGrantedAbilities(UAbilitySystemComponentV2* InAbilitySystemComponent)
{
	check(EquipmentDefinition);

	const UEquipmentDefinition* EquipmentDefinitionCDO = GetDefault<UEquipmentDefinition>(EquipmentDefinition);

	if (InAbilitySystemComponent)
	{
		for (const TObjectPtr<const UAbilitySet> AbilitySet : EquipmentDefinitionCDO->AbilitySets)
		{
			AbilitySet->GiveToAbilitySystem(InAbilitySystemComponent, &GrantedHandles, EquipmentInstance);
		}
	}
}

void FEquipmentEntry::DisableGrantedAbilities(UAbilitySystemComponentV2* InAbilitySystemComponent)
{
	if (InAbilitySystemComponent)
	{
		GrantedHandles.RemoveFromAbilitySystem(InAbilitySystemComponent);
	}
}

UEquipmentInstance* FEquipmentList::CreateEntry(TSubclassOf<UEquipmentDefinition> EquipmentDefinition)
{
	check(EquipmentDefinition);
	check(OwnerComponent);

	const UItemSubsystem* ItemSubsystem = UItemSubsystem::FindItemSubsystem(OwnerComponent->GetWorld());
	if (!ItemSubsystem)
	{
		UE_LOG(LogItem, Error, TEXT("AddEntry: Invalid Item Subsystem!"));
		return nullptr;
	}

	FEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.EquipmentDefinition = EquipmentDefinition;
	NewEntry.EquipmentInstance = ItemSubsystem->CreateEquipmentInstance(EquipmentDefinition, OwnerComponent->GetOwner());
	NewEntry.ActivateGrantedAbilities(GetAbilitySystemComponent());

	const UEquipmentDefinition* EquipmentDefinitionCDO = GetDefault<UEquipmentDefinition>(NewEntry.EquipmentDefinition);
	NewEntry.EquipmentInstance->SpawnEquipmentActors(EquipmentDefinitionCDO->EquipmentToSpawn);

	return NewEntry.EquipmentInstance;
}

void FEquipmentList::RemoveEntry(TObjectPtr<UEquipmentInstance> EquipmentInstance)
{
	check(EquipmentInstance);

	for (auto EntryIterator = Entries.CreateIterator(); EntryIterator; ++EntryIterator)
	{
		if (FEquipmentEntry& Entry = *EntryIterator; Entry.EquipmentInstance == EquipmentInstance)
		{
			Entry.DisableGrantedAbilities(GetAbilitySystemComponent());
			EquipmentInstance->DestroyEquipmentActors();
			EntryIterator.RemoveCurrent();
		}
	}
}

UAbilitySystemComponentV2* FEquipmentList::GetAbilitySystemComponent() const
{
	check(OwnerComponent);
	const AActor* OwnerActor = OwnerComponent->GetOwner();
	return Cast<UAbilitySystemComponentV2>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerActor));
}

UItemSubsystem* FEquipmentList::GetItemSubsystem() const
{
	// TODO: Remove this.
	check(OwnerComponent);
	check(GEngine);

	const ULocalPlayer* LocalPlayer = GEngine->GetFirstGamePlayer(OwnerComponent->GetWorld());
	if (!LocalPlayer)
	{
		UE_LOG(LogItem, Error, TEXT("GetItemSubsystem: Invalid Local Player!"));
		return nullptr;
	}

	return LocalPlayer->GetSubsystem<UItemSubsystem>();
}

UEquipmentManager::UEquipmentManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), EquipmentList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void UEquipmentManager::InitializeComponent()
{
	Super::InitializeComponent();
}

void UEquipmentManager::UninitializeComponent()
{
	TArray<TObjectPtr<UEquipmentInstance>> EquippedInstances;

	for (const FEquipmentEntry& Entry : EquipmentList.Entries)
	{
		EquippedInstances.Add(Entry.EquipmentInstance);
	}

	UE_LOG(LogItem, Warning, TEXT("UninitializeComponent: Unequipping %i items!"), EquippedInstances.Num());

	for (const TObjectPtr<UEquipmentInstance> EquipmentInstance : EquippedInstances)
	{
		UnequipAndDestroyItem(EquipmentInstance);
	}

	Super::UninitializeComponent();
}

UEquipmentInstance* UEquipmentManager::EquipItem(TSubclassOf<UEquipmentDefinition> EquipmentDefinition)
{
	if (!EquipmentDefinition)
	{
		UE_LOG(LogItem, Error, TEXT("EquipItem: EquipmentDefinition = NULL!"));
		return nullptr;
	}

	// Try to find existing entry before creating new one.
	FEquipmentEntry* ExistingEntry = EquipmentList.Entries.FindByPredicate([&EquipmentDefinition](const FEquipmentEntry& Entry)
	{
		return Entry.EquipmentDefinition == EquipmentDefinition;
	});

	UEquipmentInstance* Item = ExistingEntry ? ExistingEntry->EquipmentInstance : EquipmentList.CreateEntry(EquipmentDefinition);
	if (!Item)
	{
		UE_LOG(LogItem, Error, TEXT("EquipItem: Cannot create requested Item!"));
		return nullptr;
	}

	Item->OnEquipped();
	Item->SetEquipmentActorsHiddenInGame(false);
	return Item;
}

void UEquipmentManager::UnequipItem(TObjectPtr<UEquipmentInstance> EquipmentInstance)
{
	if (!EquipmentInstance)
	{
		UE_LOG(LogItem, Error, TEXT("UnequipItem: Equipment Instance = NULL!"));
		return;
	}

	EquipmentInstance->OnUnequipped();

	if (UAbilitySystemComponentV2* AbilitySystemComponentV2 = Cast<UAbilitySystemComponentV2>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner())))
	{
		for (FEquipmentEntry& Entry : EquipmentList.Entries)
		{
			if (Entry.EquipmentInstance == EquipmentInstance)
			{
				Entry.DisableGrantedAbilities(AbilitySystemComponentV2);

				// TODO: Hide equipment actors.
				Entry.EquipmentInstance->SetEquipmentActorsHiddenInGame(true);
				// TODO: do not forget to set them visible back again.
			}
		}
	}
}

void UEquipmentManager::UnequipAndDestroyItem(TObjectPtr<UEquipmentInstance> EquipmentInstance)
{
	if (!EquipmentInstance)
	{
		UE_LOG(LogItem, Error, TEXT("UnequipItemAndDestroy: Equipment Instance = NULL!"));
		return;
	}

	EquipmentInstance->OnUnequipped();
	EquipmentList.RemoveEntry(EquipmentInstance);
}
