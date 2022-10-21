// Bartender Ender
// Naughty Panda @ 2022

#include "ItemInstance.h"
#include "ItemFragment.h"
#include "ItemSubsystem.h"
#include "BartenderEnder/LogCategories.h"

UItemInstance::UItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UItemInstance::IsEquipment() const
{
	check(ItemDefinition);
	return GetDefault<UItemDefinition>(ItemDefinition)->FindFragment<UItemFragment_Equipment>() ? true : false;
}

bool UItemInstance::GiveTo(AActor* ToActor)
{
	const UItemSubsystem* ItemSubsystem = UItemSubsystem::FindItemSubsystem(GetWorld());
	if (!ItemSubsystem)
	{
		UE_LOG(LogItem, Error, TEXT("GiveTo: %s was unable to find Item Subsystem!"), *GetNameSafe(this));
		return false;
	}

	return ItemSubsystem->GiveItemTo(this, ToActor);
}

bool UItemInstance::GiveToPlayer()
{
	return GiveTo(GetWorld()->GetFirstPlayerController()->GetPawn());
}

const UItemFragment* UItemInstance::FindFragmentByClass(TSubclassOf<UItemFragment> FragmentClass) const
{
	if (ItemDefinition && FragmentClass)
	{
		return GetDefault<UItemDefinition>(ItemDefinition)->FindFragmentByClass(FragmentClass);
	}

	return nullptr;
}
