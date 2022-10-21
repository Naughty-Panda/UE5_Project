// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "AbilitySet.generated.h"

class UAbilitySystemComponentV2;
class UGameplayAbilityV2;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:
	// Gameplay Ability to add to the Ability Set.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbilityV2> Ability = nullptr;

	// Gameplay Ability level.
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	// InputTag to associate with this Gameplay Ability.
	UPROPERTY(EditDefaultsOnly, Category = "InputTag")
	FGameplayTag InputTag;
};

USTRUCT(BlueprintType)
struct FAbilitySet_GameplayEffect
{
	GENERATED_BODY()

public:
	// Gameplay Effect to add to the Ability Set.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	// Gameplay Effect level.
	UPROPERTY(EditDefaultsOnly)
	int32 EffectLevel = 1;
};

USTRUCT(BlueprintType)
struct FAbilitySet_AttributeSet
{
	GENERATED_BODY()

public:
	// AttributeSet to add to the Ability Set.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet;
};

USTRUCT(BlueprintType)
struct FAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& SpecHandle);
	void AddEffectHandle(const FActiveGameplayEffectHandle& EffectHandle);
	void AddAttributeSet(UAttributeSet* AttributeSet);

	void RemoveFromAbilitySystem(UAbilitySystemComponentV2* AbilitySystem);

protected:
	// Granted Abilities handles.
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	// Granted Effects handles.
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> EffectHandles;

	// Granted Attribute Sets pointers.
	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};

/**
 * Ability Set gives specified gameplay abilities and effects to it's owner.
 */
UCLASS(BlueprintType, Const)
class UAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void GiveToAbilitySystem(UAbilitySystemComponentV2* AbilitySystem, FAbilitySet_GrantedHandles* OutHandles, UObject* SourceObject = nullptr) const;

protected:
	// Gameplay Abilities this Ability Set provides.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", Meta = (TitleProperty = "Ability"))
	TArray<FAbilitySet_GameplayAbility> GameplayAbilities;

	// Gameplay Effects this Ability Set provides.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", Meta = (TitleProperty = "Effect"))
	TArray<FAbilitySet_GameplayEffect> GameplayEffects;

	// Attribute Sets this Ability Set provides.
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", Meta = (TitleProperty = "AttributeSet"))
	TArray<FAbilitySet_AttributeSet> AttributeSets;
};
