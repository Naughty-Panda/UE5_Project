// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbilityV2.generated.h"

class AGameCharacter;
class APlayerCharacter;

/**
 * Activation policy defines how an ability should be activated.
 */
UENUM(BlueprintType)
enum class EGameplayAbilityActivationPolicy : uint8
{
	// Tries to activate ability when input is triggered.
	OnInputTriggered,

	// Tries to activate ability while input is held.
	OnInputHeld,

	// Tries to activate ability when an avatar actor is assigned.
	OnSpawn
};

/**
 * Activation group defines how an ability activates in relation to other abilities.
 */
UENUM(BlueprintType)
enum class EGameplayAbilityActivationGroup : uint8
{
	// Ability executes independently of all other abilities.
	Independent,

	// Ability will be canceled and replaced by other exclusive abilities.
	Exclusive_Replaceable,

	// Ability will block all other exclusive abilities from activating.
	Exclusive_Blocking,

	Max UMETA(Hidden)
};

/**
 * Extended Gameplay Ability class for Bartender Ender.
 */
UCLASS(Abstract, Meta = (ShortTooltip = "Extended Gameplay Ability class for Bartender Ender"))
class UGameplayAbilityV2 : public UGameplayAbility
{
	GENERATED_BODY()
	friend class UAbilitySystemComponentV2;

public:
	UGameplayAbilityV2(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	template <typename T>
	T* GetAbilitySystemComponentFromActorInfo() const
	{
		return CurrentActorInfo ? Cast<T>(CurrentActorInfo->AbilitySystemComponent.Get()) : nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "Ability")
	APlayerController* GetPlayerControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	AController* GetControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	AGameCharacter* GetGameCharacterFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	APlayerCharacter* GetPlayerCharacterFromActorInfo() const;

	EGameplayAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
	EGameplayAbilityActivationGroup GetActivationGroup() const { return ActivationGroup; }

	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& AbilitySpec) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Ability", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool CanChangeActivationGroup(EGameplayAbilityActivationGroup ToGroup) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Ability", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool ChangeActivationGroup(EGameplayAbilityActivationGroup NewGroup);

	//UFUNCTION(BlueprintCallable, Category = "Ability")
	//void SetCameraMode(TSubclassOf<UCameraMode> CameraMode);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void ClearCameraMode();

	// Calls both native and blueprint versions of Failed To Activate event.
	void OnAbilityFailedToActivate(const FGameplayTagContainer& FailedReasons) const
	{
		NativeOnAbilityFailedToActivate(FailedReasons);
		Script_OnAbilityFailedToActivate(FailedReasons);
	}

protected:
	// Called when ability failed to activate in (Code).
	virtual void NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReasons) const;

	// Called when ability failed to activate (Blueprints).
	UFUNCTION(BlueprintImplementableEvent)
	void Script_OnAbilityFailedToActivate(const FGameplayTagContainer& FailedReasons) const;

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void SetCanBeCanceled(bool bCanBeCanceled) override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	// TODO: this
	//virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	//virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	//virtual FGameplayEffectContextHandle MakeEffectContext(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const override;
	virtual void ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec, FGameplayAbilitySpec* AbilitySpec) const override;
	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void OnPawnAvatarSet();
	//virtual void GetAbilitySource(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, float& OutSourceLevel, )

	// Called when ability is granted to the Ability System Component.
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability", DisplayName = "OnAbilityAdded")
	void Script_OnAbilityAdded();

	// Called when ability is removed from the Ability System Component.
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability", DisplayName = "OnAbilityRemoved")
	void Script_OnAbilityRemoved();

	// Called when Ability System is initialized with an Avatar Actor.
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability", DisplayName = "OnPawnAvatarSet")
	void Script_OnPawnAvatarSet();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Advanced)
	EGameplayAbilityActivationPolicy ActivationPolicy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Advanced)
	EGameplayAbilityActivationGroup ActivationGroup;

	// Ability costs

	UPROPERTY(EditDefaultsOnly, Category = Advanced)
	TMap<FGameplayTag, FText> FailureTagToErrorMessage;

	UPROPERTY(EditDefaultsOnly, Category = Advanced)
	TMap<FGameplayTag, UAnimMontage*> FailureTagToAnimMontage;

	// Camera mode
};
