// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityV2.h"
#include "AbilitySystemComponentV2.generated.h"

/**
 * Extended Ability System Component for Bartender Ender.
 */
UCLASS()
class UAbilitySystemComponentV2 : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UAbilitySystemComponentV2(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	typedef TFunctionRef<bool(const UGameplayAbilityV2* Ability, FGameplayAbilitySpecHandle SpecHandle)> TShouldCancelAbilityFunction;
	void CancelAbilitiesByFunction(TShouldCancelAbilityFunction Function);

	void CancelInputActivatedAbilities();

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();

	bool IsActivationGroupBlocked(EGameplayAbilityActivationGroup ActivationGroup) const;
	void AddAbilityToActivationGroup(EGameplayAbilityActivationGroup Group, UGameplayAbilityV2* Ability);
	void RemoveAbilityFromActivationGroup(EGameplayAbilityActivationGroup Group, UGameplayAbilityV2* Ability);
	void CancelActivationGroupAbilities(EGameplayAbilityActivationGroup Group, UGameplayAbilityV2* IgnoredAbility);

	void AddDynamicTagGameplayEffect(const FGameplayTag& Tag);
	void RemoveDynamicTagGameplayEffect(const FGameplayTag& Tag);

	//void GetAbilityTargetData(const FGameplayAbilitySpecHandle& AbilityHandle, FGameplayAbilityActivationInfo ActivationInfo, FGameplayAbilityTargetDataHandle& OutDataHandle);

	//void SetTagRelationshipMapping();

	void GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const;

protected:
	void TryActivateAbilitiesOnSpawn();

	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

	virtual void NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability) override;
	virtual void NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason) override;
	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;
	virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags,
	                                            const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags) override;
	virtual void HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bCanBeCanceled) override;

	void HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason) const;

protected:
	//UPROPERTY()
	//TagRelationshipMapping;

	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	// Number of abilities running in each activation group.
	int32 ActivationGroupCounts[EGameplayAbilityActivationGroup::Max]{};
};
