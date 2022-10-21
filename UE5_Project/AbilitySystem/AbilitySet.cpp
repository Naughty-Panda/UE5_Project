// Bartender Ender
// Naughty Panda @ 2022

#include "AbilitySet.h"
#include "AbilitySystemComponentV2.h"

void FAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& SpecHandle)
{
	if (!SpecHandle.IsValid())
	{
		return;
	}

	AbilitySpecHandles.Add(SpecHandle);
	UE_LOG(LogAbilitySystem, Warning, TEXT("Total Ability Spec Handles: %i"), AbilitySpecHandles.Num());
}

void FAbilitySet_GrantedHandles::AddEffectHandle(const FActiveGameplayEffectHandle& EffectHandle)
{
	if (!EffectHandle.IsValid())
	{
		return;
	}

	EffectHandles.Add(EffectHandle);
	UE_LOG(LogAbilitySystem, Warning, TEXT("Total Effect Handles: %i"), EffectHandles.Num());
}

void FAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* AttributeSet)
{
	GrantedAttributeSets.Add(AttributeSet);
	UE_LOG(LogAbilitySystem, Warning, TEXT("Total Attribute Sets: %i"), GrantedAttributeSets.Num());
}

void FAbilitySet_GrantedHandles::RemoveFromAbilitySystem(UAbilitySystemComponentV2* AbilitySystem)
{
	if (!AbilitySystem)
	{
		return;
	}

	if (!AbilitySystem->IsOwnerActorAuthoritative())
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("RemoveFromAbilitySystem: Owner has no authority!"));
		return;
	}

	// Remove abilities.
	for (const auto& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			AbilitySystem->ClearAbility(Handle);
		}
	}

	// Remove Effects.
	for (const auto& Handle : EffectHandles)
	{
		if (Handle.IsValid())
		{
			AbilitySystem->RemoveActiveGameplayEffect_NoReturn(Handle);
		}
	}

	// Remove Attribute sets.
	for (TObjectPtr<UAttributeSet> Set : GrantedAttributeSets)
	{
		AbilitySystem->GetSpawnedAttributes_Mutable().Remove(Set.Get());
	}

	// Clean up containers.
	AbilitySpecHandles.Reset();
	EffectHandles.Reset();
	GrantedAttributeSets.Reset();
}

UAbilitySet::UAbilitySet(const FObjectInitializer& ObjectInitializer)
{
}

void UAbilitySet::GiveToAbilitySystem(UAbilitySystemComponentV2* AbilitySystem, FAbilitySet_GrantedHandles* OutHandles, UObject* SourceObject) const
{
	if (!AbilitySystem)
	{
		return;
	}

	if (!AbilitySystem->IsOwnerActorAuthoritative())
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("AddToAbilitySystem: Owner has no authority!"));
		return;
	}

	// Add Abilities.
	for (const FAbilitySet_GameplayAbility& AbilityToAdd : GameplayAbilities)
	{
		if (!IsValid(AbilityToAdd.Ability))
		{
			UE_LOG(LogAbilitySystem, Error, TEXT("AddToAbilitySystem: Invalid ability on ability set %s"), *GetNameSafe(this));
			continue;
		}

		UGameplayAbilityV2* AbilityCDO = AbilityToAdd.Ability->GetDefaultObject<UGameplayAbilityV2>();
		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToAdd.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToAdd.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = AbilitySystem->GiveAbility(AbilitySpec);

		if (OutHandles)
		{
			OutHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	// Add Effects.
	for (const FAbilitySet_GameplayEffect& EffectToAdd : GameplayEffects)
	{
		if (!IsValid(EffectToAdd.GameplayEffect))
		{
			UE_LOG(LogAbilitySystem, Error, TEXT("AddToAbilitySystem: Invalid effect on ability set %s"), *GetNameSafe(this));
			continue;
		}

		const UGameplayEffect* GameplayEffect = EffectToAdd.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = AbilitySystem->ApplyGameplayEffectToSelf(GameplayEffect, EffectToAdd.EffectLevel, AbilitySystem->MakeEffectContext());

		if (OutHandles)
		{
			OutHandles->AddEffectHandle(GameplayEffectHandle);
		}
	}

	// Add Attribute Sets.
	for (const FAbilitySet_AttributeSet& AttributeSetToAdd : AttributeSets)
	{
		if (!IsValid(AttributeSetToAdd.AttributeSet))
		{
			UE_LOG(LogAbilitySystem, Error, TEXT("AddToAbilitySystem: Invalid attribute set on ability set %s"), *GetNameSafe(this));
			continue;
		}

		UAttributeSet* NewAttributeSet = NewObject<UAttributeSet>(AbilitySystem->GetOwner(), AttributeSetToAdd.AttributeSet);
		AbilitySystem->AddAttributeSetSubobject(NewAttributeSet);

		if (OutHandles)
		{
			OutHandles->AddAttributeSet(NewAttributeSet);
		}
	}
}
