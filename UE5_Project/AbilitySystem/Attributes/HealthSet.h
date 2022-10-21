// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "AbilitySystemComponent.h"
#include "BaseAttributeSet.h"
#include "HealthSet.generated.h"

/**
 * Character Health Attribute Set for Bartender Ender.
 */
UCLASS(BlueprintType)
class BARTENDERENDER_API UHealthSet : public UBaseAttributeSet
{
	GENERATED_BODY()

public:
	UHealthSet();

	ATTRIBUTE_ACCESSORS(UHealthSet, Health);
	ATTRIBUTE_ACCESSORS(UHealthSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UHealthSet, Healing);

protected:
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue);

public:
	// Fires when its time to die.
	mutable FAttributeEvent OnOutOfHealth;

private:
	// Current Health.
	UPROPERTY(BlueprintReadOnly, Category = "Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	// Current MaxHealth.
	UPROPERTY(BlueprintReadOnly, Category = "Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	// Incoming Healing, adds Health.
	UPROPERTY(BlueprintReadOnly, Category = "Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Healing;

	// Incoming Damage, subtracts Health.
	UPROPERTY(BlueprintReadOnly, Category = "Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Damage;

	bool bOutOfHealth = false;
};
