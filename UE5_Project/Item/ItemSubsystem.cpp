// Bartender Ender
// Naughty Panda @ 2022

#include "ItemSubsystem.h"
#include "ItemInstance.h"
#include "BartenderEnder/LogCategories.h"
#include "BartenderEnder/Inventory/EquipmentSlots.h"
#include "BartenderEnder/Inventory/InventoryManager.h"
#include "Equipment/EquipmentDefinition.h"
#include "Equipment/EquipmentInstance.h"
#include "Equipment/EquipmentManager.h"

UItemSubsystem::UItemSubsystem()
	: Super()
{
}

void UItemSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogItem, Warning, TEXT("Item Subsystem initialized!"));
}

void UItemSubsystem::Deinitialize()
{
	UE_LOG(LogItem, Warning, TEXT("Item Subsystem deinitialized!"));
	UE_LOG(LogItem, Warning, TEXT("%i actors have not been cleared!"), SpawnedActors.Num());
	SpawnedActors.Empty();

	Super::Deinitialize();
}

bool UItemSubsystem::GiveItemTo(UItemInstance* Item, AActor* ToActor) const
{
	if (!Item || !ToActor)
	{
		return false;
	}

	if (!Item->IsEquipment())
	{
		// Item is not equipment - try to add to inventory.
		const UInventoryManager* InventoryManager = ToActor->FindComponentByClass<UInventoryManager>();
		// TODO: Check free space.
		if (!InventoryManager)
		{
			return false;
		}
	}

	// Item is equipment - try to equip it.
	UEquipmentSlots* EquipmentSlots = ToActor->FindComponentByClass<UEquipmentSlots>();
	const UEquipmentManager* EquipmentManager = ToActor->FindComponentByClass<UEquipmentManager>();

	const int32 ExistingItemSlot = EquipmentSlots ? EquipmentSlots->GetItemSlot(Item) : INDEX_NONE;
	const int32 EmptySlotIndex = EquipmentSlots ? EquipmentSlots->GetNextEmptySlot() : INDEX_NONE;

	if (!EquipmentManager || !EquipmentSlots)
	{
		// Cannot equip item.
		return false;
	}

	if (ExistingItemSlot != INDEX_NONE)
	{
		// Item already exists in inventory - setting it active.
		// TODO: add stacks later?
		EquipmentSlots->SetActiveSlotIndex(ExistingItemSlot);
		return true;
	}

	EquipmentSlots->AddItemToSlot(Item, EmptySlotIndex);
	EquipmentSlots->SetActiveSlotIndex(EmptySlotIndex);

	return true;
}

bool UItemSubsystem::GiveItemToPlayer(UItemInstance* Item) const
{
	return GiveItemTo(Item, GetWorld()->GetFirstPlayerController()->GetPawn());
}

UItemSubsystem* UItemSubsystem::FindItemSubsystem(TObjectPtr<UWorld> InWorld)
{
	check(InWorld);

	const ULocalPlayer* LocalPlayer = InWorld->GetFirstLocalPlayerFromController();
	if (!LocalPlayer)
	{
		UE_LOG(LogItem, Error, TEXT("FindItemSubsystem: Invalid Local Player!"));
		return nullptr;
	}

	return LocalPlayer->GetSubsystem<UItemSubsystem>();
}

UItemInstance* UItemSubsystem::CreateItemInstance(TObjectPtr<UObject> Owner) const
{
	return NewObject<UItemInstance>(Owner);
}

UEquipmentInstance* UItemSubsystem::CreateEquipmentInstance(TSubclassOf<UEquipmentDefinition> EquipmentDefinition, TObjectPtr<UObject> Owner) const
{
	check(EquipmentDefinition);

	const UEquipmentDefinition* DefinitionCDO = GetDefault<UEquipmentDefinition>(EquipmentDefinition);
	const TSubclassOf<UEquipmentInstance> EquipmentInstanceType = DefinitionCDO->EquipmentInstance ? DefinitionCDO->EquipmentInstance : UEquipmentInstance::StaticClass();

	return NewObject<UEquipmentInstance>(Owner, EquipmentInstanceType);
}

AActor* UItemSubsystem::SpawnEquipmentActor(TSubclassOf<AActor> ActorSubclass, TObjectPtr<AActor> Owner) const
{
	AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(ActorSubclass, FTransform::Identity, Owner);
	NewActor->FinishSpawning(FTransform::Identity, true);

	// Add a weak pointer to spawned actors array.
	SpawnedActors.Add(NewActor);

	return NewActor;
}

void UItemSubsystem::DestroyEquipmentActor(TObjectPtr<AActor> EquipmentActor)
{
	if (EquipmentActor)
	{
		// Remove weak pointer from spawned actors array.
		SpawnedActors.RemoveSingle(EquipmentActor);

		EquipmentActor->Destroy();
	}
}

UEquipmentManager* UItemSubsystem::GetPlayerEquipmentManager() const
{
	const ULocalPlayer* LocalPlayer = GetLocalPlayer<ULocalPlayer>();
	if (!LocalPlayer)
	{
		UE_LOG(LogItem, Error, TEXT("GetPlayerEquipmentManager: Invalid Local Player!"));
		return nullptr;
	}

	const APlayerController* PlayerController = LocalPlayer->GetPlayerController(GetWorld());
	if (!PlayerController)
	{
		UE_LOG(LogItem, Error, TEXT("GetPlayerEquipmentManager: Invalid Player Controller!"));
		return nullptr;
	}

	const APawn* PlayerPawn = PlayerController->GetPawn();
	if (!PlayerPawn)
	{
		UE_LOG(LogItem, Error, TEXT("GetPlayerEquipmentManager: Invalid Player Pawn!"));
		return nullptr;
	}

	return PlayerPawn->FindComponentByClass<UEquipmentManager>();
}

UEquipmentInstance* UItemSubsystem::EquipItemOnPlayer(TSubclassOf<UEquipmentDefinition> EquipmentDefinition) const
{
	UEquipmentManager* EquipmentManager = GetPlayerEquipmentManager();
	if (!EquipmentManager)
	{
		UE_LOG(LogItem, Error, TEXT("EquipItemOnPlayer: Invalid Player Equipment Manager!"));
		return nullptr;
	}

	return EquipmentManager->EquipItem(EquipmentDefinition);
}

void UItemSubsystem::UnequipItemFromPlayer(UEquipmentInstance* EquippedItem, bool bDestroyItem) const
{
	UEquipmentManager* EquipmentManager = GetPlayerEquipmentManager();
	if (!EquipmentManager)
	{
		UE_LOG(LogItem, Error, TEXT("UnequipItemFromPlayer: Invalid Player Equipment Manager!"));
		return;
	}

	bDestroyItem ? EquipmentManager->UnequipAndDestroyItem(EquippedItem) : EquipmentManager->UnequipItem(EquippedItem);
}
