// Bartender Ender
// Naughty Panda @ 2022

#include "EquipmentDefinition.h"
#include "EquipmentInstance.h"

UEquipmentDefinition::UEquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	EquipmentInstance = UEquipmentInstance::StaticClass();
}
