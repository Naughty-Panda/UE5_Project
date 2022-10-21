// Bartender Ender
// Naughty Panda @ 2022

#include "PlayerCharacter.h"
#include "BartenderEnder/LogCategories.h"
#include "BartenderEnder/Components/PlayerPawnComponent.h"
#include "BartenderEnder/Inventory/EquipmentSlots.h"
#include "BartenderEnder/Inventory/InventoryManager.h"
#include "BartenderEnder/Item/Equipment/EquipmentManager.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerPawnComponent = CreateDefaultSubobject<UPlayerPawnComponent>(TEXT("PlayerPawnComponent"));

	InventoryManager = CreateDefaultSubobject<UInventoryManager>(TEXT("InventoryManager"));
	EquipmentManager = CreateDefaultSubobject<UEquipmentManager>(TEXT("EquipmentManager"));

	// Equipment slots.
	PrimaryEquipmentSlots = CreateDefaultSubobject<UEquipmentSlots>(TEXT("PrimaryEquipmentSlots"));
	PrimaryEquipmentSlots->SetEquipmentType(EEquipmentType::Primary);

	SecondaryEquipmentSlots = CreateDefaultSubobject<UEquipmentSlots>(TEXT("SecondaryEquipmentSlots"));
	SecondaryEquipmentSlots->SetEquipmentType(EEquipmentType::Secondary);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	CheckReadyToInitialize();
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	UE_LOG(LogCharacter, Warning, TEXT("APlayerCharacter::PossessedBy %s"), *GetNameSafe(NewController));
	Super::PossessedBy(NewController);
}
