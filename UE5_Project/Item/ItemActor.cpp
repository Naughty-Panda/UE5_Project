// Bartender Ender
// Naughty Panda @ 2022

#include "ItemActor.h"
#include "ItemInstance.h"
#include "ItemSubsystem.h"
#include "BartenderEnder/LogCategories.h"
#include "Equipment/EquipmentDefinition.h"

AItemActor::AItemActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(RootComponent);
}

void AItemActor::CollectInteractionOptions(const FInteractionQuery& InteractionQuery, FInteractionOptionBuilder& OptionBuilder)
{
	OptionBuilder.AddInteractionOption(InteractionOption);
}

void AItemActor::BeginPlay()
{
	Super::BeginPlay();
}

UItemInstance* AItemActor::SpawnItem()
{
	if (ItemInstance)
	{
		UE_LOG(LogItem, Error, TEXT("SpawnItem: %s: ItemInstance has already been created!"), *GetNameSafe(this));
		return nullptr;
	}

	if (!ItemDefinition)
	{
		UE_LOG(LogItem, Error, TEXT("SpawnItem: %s: Invalid Item Definition!"), *GetNameSafe(this));
		return nullptr;
	}

	const UItemSubsystem* ItemSubsystem = UItemSubsystem::FindItemSubsystem(GetWorld());
	if (!ItemSubsystem)
	{
		UE_LOG(LogItem, Error, TEXT("SpawnItem: %s: Invalid Item Subsystem!"), *GetNameSafe(this));
		return nullptr;
	}

	ItemInstance = ItemSubsystem->CreateItemInstance(this);
	ItemInstance->SetItemDefinition(ItemDefinition);

	UpdateItemMesh();

	return ItemInstance;

	if (!ItemInstance->IsEquipment())
	{
		// Not an equipment - currently nothing to do.
		UE_LOG(LogItem, Warning, TEXT("SpawnItem: %s is not equipment."), *GetNameSafe(this));
		return ItemInstance;
	}

	// Item is equipment
	const TSubclassOf<UEquipmentDefinition> EquipmentDefinition = ItemInstance->FindFragment<UItemFragment_Equipment>()->EquipmentDefinition;
	const UEquipmentInstance* EquipmentInstance = ItemSubsystem->CreateEquipmentInstance(EquipmentDefinition, this);
	return ItemInstance;
}

void AItemActor::UpdateItemMesh()
{
	if (!ItemInstance || !ItemInstance->IsEquipment() || !ItemMesh)
	{
		return;
	}

	const TSubclassOf<UEquipmentDefinition> EquipmentDefinitionClass = ItemInstance->FindFragment<UItemFragment_Equipment>()->EquipmentDefinition;
	check(EquipmentDefinitionClass);

	const UEquipmentDefinition* EquipmentDefinition = GetDefault<UEquipmentDefinition>(EquipmentDefinitionClass);
	if (!EquipmentDefinition || !EquipmentDefinition->EquipmentToSpawn.IsValidIndex(0))
	{
		return;
	}

	const auto& [ActorToSpawn, AttachmentSocket, AttachmentTransform] = EquipmentDefinition->EquipmentToSpawn[0];
	const AActor* EquipmentActorToSpawn = GetDefault<AActor>(ActorToSpawn);
	if (!EquipmentActorToSpawn)
	{
		return;
	}

	const UStaticMeshComponent* ActorMeshComponent = EquipmentActorToSpawn->FindComponentByClass<UStaticMeshComponent>();
	if (!ActorMeshComponent)
	{
		return;
	}

	ItemMesh->SetStaticMesh(ActorMeshComponent->GetStaticMesh());
}
