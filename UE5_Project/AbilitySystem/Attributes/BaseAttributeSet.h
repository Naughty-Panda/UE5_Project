// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "BaseAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(SetName, AttributeName)\
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(SetName, AttributeName)\
	GAMEPLAYATTRIBUTE_VALUE_GETTER(AttributeName)\
	GAMEPLAYATTRIBUTE_VALUE_SETTER(AttributeName)\
	GAMEPLAYATTRIBUTE_VALUE_INITTER(AttributeName)

// Delegate used to broadcast attribute events.
DECLARE_MULTICAST_DELEGATE_FourParams(FAttributeEvent, AActor* Instigator, AActor* Causer, const struct FGameplayEffectSpec& EffectSpec, float Value);

/**
 * Base class for Bartender Ender Attribute Sets.
 */
UCLASS()
class BARTENDERENDER_API UBaseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UBaseAttributeSet();
};
