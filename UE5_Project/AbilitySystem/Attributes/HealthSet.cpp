// Bartender Ender
// Naughty Panda @ 2022

#include "HealthSet.h"

UHealthSet::UHealthSet()
	: Health(100.f), MaxHealth(100.f), Healing(0.f), Damage(0.f), bOutOfHealth(false)
{
}

bool UHealthSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	UE_LOG(LogAbilitySystem, Log, TEXT("HealthSet: PreGameplayEffectExecute"));
	return Super::PreGameplayEffectExecute(Data);
}

void UHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	UE_LOG(LogAbilitySystem, Log, TEXT("HealthSet: PostGameplayEffectExecute"));
	Super::PostGameplayEffectExecute(Data);
}

void UHealthSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	UE_LOG(LogAbilitySystem, Log, TEXT("HealthSet: PreAttributeBaseChange"));
	Super::PreAttributeBaseChange(Attribute, NewValue);
}

void UHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	UE_LOG(LogAbilitySystem, Log, TEXT("HealthSet: PreAttributeChange"));

	Super::PreAttributeChange(Attribute, NewValue);
}

void UHealthSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	UE_LOG(LogAbilitySystem, Log, TEXT("HealthSet: PostAttributeChange"));

	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}

void UHealthSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue)
{
	// TODO: this
}
