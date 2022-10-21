// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "ItemSubsystem.generated.h"

class UItemInstance;
class UEquipmentInstance;
class UEquipmentDefinition;
class UEquipmentManager;

/**
 * Game Item Subsystem.
 */
UCLASS()
class UItemSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	UItemSubsystem();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	// Gives an Item to specified Actor.
	UFUNCTION(BlueprintCallable, Category = "Item")
	bool GiveItemTo(UItemInstance* Item, AActor* ToActor) const;

	// Gives an Item to the Player.
	UFUNCTION(BlueprintCallable, Category = "Item")
	bool GiveItemToPlayer(UItemInstance* Item) const;

	UE_NODISCARD static UItemSubsystem* FindItemSubsystem(TObjectPtr<UWorld> InWorld);
	UE_NODISCARD UItemInstance* CreateItemInstance(TObjectPtr<UObject> Owner) const;
	UE_NODISCARD UEquipmentInstance* CreateEquipmentInstance(TSubclassOf<UEquipmentDefinition> EquipmentDefinition, TObjectPtr<UObject> Owner) const;
	UE_NODISCARD AActor* SpawnEquipmentActor(TSubclassOf<AActor> ActorSubclass, TObjectPtr<AActor> Owner) const;
	void DestroyEquipmentActor(TObjectPtr<AActor> EquipmentActor);

	// Returns Player Equipment Manager.
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Equipment")
	UEquipmentManager* GetPlayerEquipmentManager() const;

	// Uses Player Equipment Manager to equip given item.
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Equipment")
	UEquipmentInstance* EquipItemOnPlayer(TSubclassOf<UEquipmentDefinition> EquipmentDefinition) const;

	// Uses Player Equipment Manager to unequip item.
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Equipment")
	void UnequipItemFromPlayer(UEquipmentInstance* EquippedItem, bool bDestroyItem) const;

private:
	mutable TArray<TWeakObjectPtr<AActor>> SpawnedActors;
};
