// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "BartenderEnder/CoreTypes.h"
#include "Components/GameFrameworkComponent.h"
#include "HealthComponent.generated.h"

class UHealthComponent;
class UAbilitySystemComponentV2;
class UHealthSet;
struct FOnAttributeChangeData;
struct FGameplayEffectSpec;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeathEvent, AActor*, Owner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FHealthAttributeChanged, UHealthComponent*, HealthComponent, AActor*, Instigator, float, OldValue, float, NewValue);

/**
 * Actor's health component.
 * Handles everything related to actor's health.
 */

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BARTENDERENDER_API UHealthComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()

public:
	UHealthComponent(const FObjectInitializer& ObjectInitializer);

	// Tries to find Health Component for given Actor.
	UFUNCTION(BlueprintPure, Category = "Health")
	static UHealthComponent* FindHealthComponent(const AActor* ForActor) { return ForActor ? ForActor->FindComponentByClass<UHealthComponent>() : nullptr; }

	// Ability System Initialization.
	UFUNCTION(BlueprintCallable, Category = "Health")
	bool TryInitializeWithAbilitySystem(UAbilitySystemComponentV2* InAbilitySystemComponent);

	// Ability System Uninitialization.
	UFUNCTION(BlueprintCallable, Category = "Health")
	void UninitializeFromAbilitySystem();

	// Returns current Health value from Ability System.
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealth() const;

	// Returns current Max Health value from Ability System.
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetMaxHealth() const;

	// Returns normalized current Health value in range (0-1).
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealthNormalized() const;

	// Returns current Death state of this component's owner.
	UFUNCTION(BlueprintCallable, Category = "Health")
	EDeathState GetDeathState() const { return DeathState; }

	// Checks if the component's owner should be dead or dying.
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Health", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsDeadOrDying() const { return DeathState > EDeathState::NotDead; }

	// Managing Death State
	virtual void StartDeath();
	virtual void FinishDeath();

protected:
	virtual void BeginPlay() override;
	virtual void OnUnregister() override;

	void ClearGameplayTags() const;

	virtual void HealthChangedHelper(const FOnAttributeChangeData& ChangeData);
	virtual void MaxHealthChangedHelper(const FOnAttributeChangeData& ChangeData);
	virtual void OutOfHealthHelper(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageValue);

private:
	void ApplyDeathState_Internal(EDeathState NewDeathState);

public:
	UPROPERTY(BlueprintAssignable)
	FDeathEvent OnDying;

	UPROPERTY(BlueprintAssignable)
	FDeathEvent OnDeath;

	UPROPERTY(BlueprintAssignable)
	FHealthAttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FHealthAttributeChanged OnMaxHealthChanged;

private:
	UPROPERTY()
	UAbilitySystemComponentV2* AbilitySystemComponent = nullptr;

	UPROPERTY()
	const UHealthSet* HealthSet = nullptr;

	UPROPERTY()
	EDeathState DeathState = EDeathState::NotDead;
};
