// Bartender Ender
// Naughty Panda @ 2022

#include "ItemDefinition.h"
#include "ItemFragment.h"
#include "BartenderEnder/LogCategories.h"

UItemDefinition::UItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UItemFragment* UItemDefinition::FindFragmentByClass(TSubclassOf<UItemFragment> FragmentClass) const
{
	if (!FragmentClass)
	{
		UE_LOG(LogItem, Error, TEXT("FindFragmentByClass: Fragment class = NULL!"));
		return nullptr;
	}

	for (const UItemFragment* Fragment : Fragments)
	{
		if (Fragment && Fragment->IsA(FragmentClass))
		{
			return Fragment;
		}
	}

	return nullptr;
}
