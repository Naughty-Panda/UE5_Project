// Bartender Ender
// Naughty Panda @ 2022

#include "IPickupable.h"
#include "Abilities/GameplayAbility.h"
#include "BartenderEnder/LogCategories.h"
#include "BartenderEnder/Item/ItemSubsystem.h"

TScriptInterface<IPickupable> UPickupableStatics::GetIPickupableFromActorInfo(UGameplayAbility* Ability)
{
	const FGameplayAbilityActorInfo* AbilityActorInfo = Ability->GetCurrentActorInfo();
	AActor* Avatar = AbilityActorInfo->AvatarActor.Get();

	// Return ability's avatar if it is pickupable.
	if (TScriptInterface<IPickupable> PickupableActor(Avatar); PickupableActor)
	{
		return PickupableActor;
	}

	// Check avatar's components for pickupable interface.
	TArray<UActorComponent*> PickupableComponents = Avatar ? Avatar->GetComponentsByInterface(UPickupable::StaticClass()) : TArray<UActorComponent*>();
	if (PickupableComponents.Num() > 0)
	{
		ensureMsgf(PickupableComponents.Num() == 1, TEXT("%s has more than 1 pickupable component! Currently we support only the fist one."), *GetNameSafe(Avatar));
		return TScriptInterface<IPickupable>(PickupableComponents[0]);
	}

	return TScriptInterface<IPickupable>();
}

void UPickupableStatics::AddPickupInventory(AActor* ReceivingActor, TScriptInterface<IPickupable> PickupableActor)
{
	if (!ReceivingActor || !PickupableActor)
	{
		return;
	}

	const FPickupInventory& PickupInventory = PickupableActor->GetPickupInventory();

	const UItemSubsystem* ItemSubsystem = UItemSubsystem::FindItemSubsystem(ReceivingActor->GetWorld());
	if (!ItemSubsystem)
	{
		UE_LOG(LogItem, Error, TEXT("AddPickupInventory: No Item Subsystem for %s!"), *GetNameSafe(ReceivingActor));
		return;
	}

	for (const FPickupTemplate& ItemTemplate : PickupInventory.ItemTemplates)
	{
		// TODO: Add item definitions.
		UE_LOG(LogItem, Error, TEXT("AddPickupInventory: ItemTemplate is not implemented yet! Receiving Actor: %s!"), *GetNameSafe(ReceivingActor));
	}

	for (const auto& Item : PickupInventory.Items)
	{
		ItemSubsystem->GiveItemTo(Item, ReceivingActor);
	}
}
