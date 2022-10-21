// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItemFragment.generated.h"

class UEquipmentDefinition;
class UItemInstance;

/**
 * Base Item Fragment.
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class UItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnItemCreated(TObjectPtr<UItemInstance> ItemInstance) const {}
};

/**
 * Equippable Item Fragment.
 */
UCLASS()
class UItemFragment_Equipment : public UItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Equipment")
	TSubclassOf<UEquipmentDefinition> EquipmentDefinition;
};

/**
 * Reticle Config Fragment.
 */
UCLASS()
class UItemFragment_ReticleConfig : public UItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reticle")
	TArray<TSubclassOf<UUserWidget>> ReticleWidgets;
};
