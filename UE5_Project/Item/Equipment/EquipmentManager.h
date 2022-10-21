// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "BartenderEnder/AbilitySystem/AbilitySet.h"
#include "Components/ActorComponent.h"
#include "EquipmentManager.generated.h"

class UEquipmentDefinition;
class UEquipmentInstance;
class UItemSubsystem;

/**
 * Entry of an equipment stored in Equipment List.
 */
USTRUCT(BlueprintType)
struct FEquipmentEntry
{
	GENERATED_BODY()

private:
	friend struct FEquipmentList;
	friend class UEquipmentManager;

private:
	void ActivateGrantedAbilities(UAbilitySystemComponentV2* InAbilitySystemComponent);
	void DisableGrantedAbilities(UAbilitySystemComponentV2* InAbilitySystemComponent);

private:
	// This equipment subclass.
	UPROPERTY()
	TSubclassOf<UEquipmentDefinition> EquipmentDefinition;

	// Equipment Instance.
	UPROPERTY()
	TObjectPtr<UEquipmentInstance> EquipmentInstance;

	// List of Ability Handles.
	UPROPERTY()
	FAbilitySet_GrantedHandles GrantedHandles;
};

/**
 * List of stored equipment.
 */
USTRUCT(BlueprintType)
struct FEquipmentList
{
	GENERATED_BODY()

public:
	FEquipmentList() : OwnerComponent(nullptr)
	{
	}

	FEquipmentList(TObjectPtr<UActorComponent> InOwnerComponent) : OwnerComponent(InOwnerComponent)
	{
	}

public:
	void AddEntry(TObjectPtr<UEquipmentInstance> EquipmentInstance);
	UE_NODISCARD UEquipmentInstance* CreateEntry(TSubclassOf<UEquipmentDefinition> EquipmentDefinition);
	void RemoveEntry(TObjectPtr<UEquipmentInstance> EquipmentInstance);
	// TODO: Separate func to disable entry?

private:
	UE_NODISCARD UAbilitySystemComponentV2* GetAbilitySystemComponent() const;
	UE_NODISCARD UItemSubsystem* GetItemSubsystem() const;

private:
	friend class UEquipmentManager;

private:
	// Pointer to owner component.
	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;

	// List of equipment entries.
	UPROPERTY()
	TArray<FEquipmentEntry> Entries;
};

/**
 * Equipment Manager for Items that can be equipped by a Pawn.
 */
UCLASS(BlueprintType, Const, Meta=(BlueprintSpawnableComponent))
class UEquipmentManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UEquipmentManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;

	UE_NODISCARD UEquipmentInstance* EquipItem(TSubclassOf<UEquipmentDefinition> EquipmentDefinition);
	void UnequipItem(TObjectPtr<UEquipmentInstance> EquipmentInstance);
	void UnequipAndDestroyItem(TObjectPtr<UEquipmentInstance> EquipmentInstance);

private:
	FEquipmentList EquipmentList;
};
