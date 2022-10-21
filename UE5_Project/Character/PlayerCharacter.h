// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "GameCharacter.h"
#include "PlayerCharacter.generated.h"

class UPlayerPawnComponent;
class UInventoryManager;
class UEquipmentManager;
class UEquipmentSlots;

/**
 * Base player character class with input support.
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "Base class for Player character with input support."))
class APlayerCharacter : public AGameCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;

protected:
	// PlayerPawnComponent allows pawn to receive player input.
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Character", Meta = (AllowPrivateAccess = true))
	UPlayerPawnComponent* PlayerPawnComponent = nullptr;

	// Player Inventory Manager Component.
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Inventory", Meta = (AllowPrivateAccess = true))
	TObjectPtr<UInventoryManager> InventoryManager;

	// Player Equipment Manager Component.
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Inventory", Meta = (AllowPrivateAccess = true))
	TObjectPtr<UEquipmentManager> EquipmentManager;

	// Primary Equipment Slots for the Player.
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Inventory", Meta = (AllowPrivateAccess = true))
	TObjectPtr<UEquipmentSlots> PrimaryEquipmentSlots;

	// Secondary Equipment Slots for the Player.
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Inventory", Meta = (AllowPrivateAccess = true))
	TObjectPtr<UEquipmentSlots> SecondaryEquipmentSlots;
};
