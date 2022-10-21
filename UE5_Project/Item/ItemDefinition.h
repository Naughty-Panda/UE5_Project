// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "ItemFragment.h"
#include "UObject/Object.h"
#include "ItemDefinition.generated.h"

class UItemFragment;

/**
 * Bartender Ender Item Definition.
 */
UCLASS(Blueprintable, Const, Abstract)
class UItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Returns Item fragment with specified class.
	template <typename T>
	const T* FindFragment() const;

	// Returns Item fragment with specified class.
	const UItemFragment* FindFragmentByClass(TSubclassOf<UItemFragment> FragmentClass) const;

public:
	// Item Name.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Display")
	FText DisplayName;

	// Item Fragments.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Display")
	TArray<TObjectPtr<UItemFragment>> Fragments;
};

template <typename T>
const T* UItemDefinition::FindFragment() const
{
	for (const UItemFragment* Fragment : Fragments)
	{
		if (Fragment && Fragment->IsA<T>())
		{
			return Cast<T>(Fragment);
		}
	}

	return nullptr;
}
