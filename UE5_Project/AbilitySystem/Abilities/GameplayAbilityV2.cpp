// Bartender Ender
// Naughty Panda @ 2022

#include "GameplayAbilityV2.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "BartenderEnder/CoreGameplayTags.h"
#include "BartenderEnder/AbilitySystem/AbilitySystemComponentV2.h"
#include "BartenderEnder/Character/GameCharacter.h"
#include "BartenderEnder/Character/PlayerCharacter.h"

UGameplayAbilityV2::UGameplayAbilityV2(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	//NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	//NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
	ActivationPolicy = EGameplayAbilityActivationPolicy::OnInputTriggered;
	ActivationGroup = EGameplayAbilityActivationGroup::Independent;
}

//UAbilitySystemComponent* UGameplayAbilityV2::GetAbilitySystemComponentFromActorInfo() const
//{
//	return CurrentActorInfo ? CurrentActorInfo->AbilitySystemComponent.Get() : nullptr;
//}

APlayerController* UGameplayAbilityV2::GetPlayerControllerFromActorInfo() const
{
	return CurrentActorInfo ? CurrentActorInfo->PlayerController.Get() : nullptr;
}

AController* UGameplayAbilityV2::GetControllerFromActorInfo() const
{
	if (!CurrentActorInfo)
	{
		return nullptr;
	}

	if (AController* Controller = CurrentActorInfo->PlayerController.Get())
	{
		return Controller;
	}

	// Check other owner if no controller was found.
	AActor* Owner = CurrentActorInfo->OwnerActor.Get();
	while (Owner)
	{
		if (AController* Controller = Cast<AController>(Owner))
		{
			return Controller;
		}

		if (const APawn* Pawn = Cast<APawn>(Owner))
		{
			return Pawn->GetController();
		}

		Owner = Owner->GetOwner();
	}

	return nullptr;
}

AGameCharacter* UGameplayAbilityV2::GetGameCharacterFromActorInfo() const
{
	return CurrentActorInfo ? Cast<AGameCharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr;
}

APlayerCharacter* UGameplayAbilityV2::GetPlayerCharacterFromActorInfo() const
{
	return CurrentActorInfo ? Cast<APlayerCharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr;
}

void UGameplayAbilityV2::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& AbilitySpec) const
{
	const bool bIsPredicting = AbilitySpec.ActivationInfo.ActivationMode == EGameplayAbilityActivationMode::Predicting;

	// Try to activate Ability if Activation Policy is OnSpawn.
	if (ActorInfo && !AbilitySpec.IsActive() && !bIsPredicting && (ActivationPolicy == EGameplayAbilityActivationPolicy::OnSpawn))
	{
		UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

		if (AbilitySystemComponent && AvatarActor && !AvatarActor->GetTearOff() && (AvatarActor->GetLifeSpan() <= 0.f))
		{
			const bool bIsLocalExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted) || (NetExecutionPolicy ==
				EGameplayAbilityNetExecutionPolicy::LocalOnly);
			const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;

			if (bClientShouldActivate)
			{
				AbilitySystemComponent->TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

bool UGameplayAbilityV2::CanChangeActivationGroup(EGameplayAbilityActivationGroup ToGroup) const
{
	if (!IsInstantiated() || !IsActive())
	{
		return false;
	}

	if (ActivationGroup == ToGroup)
	{
		return true;
	}

	const UAbilitySystemComponentV2* AbilitySystemComponentV2 = GetAbilitySystemComponentFromActorInfo<UAbilitySystemComponentV2>();
	check(AbilitySystemComponentV2);

	// If ability is blocked - it can't change Activation Group.
	if ((ActivationGroup != EGameplayAbilityActivationGroup::Exclusive_Blocking) && (AbilitySystemComponentV2->IsActivationGroupBlocked(ToGroup)))
	{
		return false;
	}

	// If ability can't be canceled - it can't become replaceable.
	if ((ToGroup == EGameplayAbilityActivationGroup::Exclusive_Replaceable) && !CanBeCanceled())
	{
		return false;
	}

	return true;
}

bool UGameplayAbilityV2::ChangeActivationGroup(EGameplayAbilityActivationGroup NewGroup)
{
	ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(ChangeActivationGroup, false);

	if (!CanChangeActivationGroup(NewGroup))
	{
		return false;
	}

	if (ActivationGroup != NewGroup)
	{
		UAbilitySystemComponentV2* AbilitySystemComponentV2 = GetAbilitySystemComponentFromActorInfo<UAbilitySystemComponentV2>();
		check(AbilitySystemComponentV2);

		AbilitySystemComponentV2->RemoveAbilityFromActivationGroup(ActivationGroup, this);
		AbilitySystemComponentV2->AddAbilityToActivationGroup(NewGroup, this);

		ActivationGroup = NewGroup;
	}

	return true;
}

void UGameplayAbilityV2::ClearCameraMode()
{
	ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(ClearCameraMode,);

	// TODO: this
}

void UGameplayAbilityV2::NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReasons) const
{
	// TODO: NativeOnAbilityFailedToActivate
}

bool UGameplayAbilityV2::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                            const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return false;
	}

	const UAbilitySystemComponentV2* AbilitySystemComponentV2 = CastChecked<UAbilitySystemComponentV2>(ActorInfo->AbilitySystemComponent.Get());
	const FCoreGameplayTags& CoreGameplayTags = FCoreGameplayTags::Get();

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (AbilitySystemComponentV2->IsActivationGroupBlocked(ActivationGroup))
	{
		if (OptionalRelevantTags)
		{
			// TODO: this
			// OptionalRelevantTags->AddTag(CoreGameplayTags.Ability_ActivateFail_ActivationGroup);
		}

		return false;
	}

	return true;
}

void UGameplayAbilityV2::SetCanBeCanceled(bool bCanBeCanceled)
{
	if (!bCanBeCanceled && (ActivationGroup == EGameplayAbilityActivationGroup::Exclusive_Replaceable))
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("%s cannot block canceling: activation group = replacable."), *GetName());
		return;
	}

	Super::SetCanBeCanceled(bCanBeCanceled);
}

void UGameplayAbilityV2::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	Script_OnAbilityAdded();
	TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

void UGameplayAbilityV2::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Script_OnAbilityRemoved();
	Super::OnRemoveAbility(ActorInfo, Spec);
}

void UGameplayAbilityV2::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGameplayAbilityV2::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                    bool bReplicateEndAbility, bool bWasCancelled)
{
	ClearCameraMode();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGameplayAbilityV2::ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec, FGameplayAbilitySpec* AbilitySpec) const
{
	Super::ApplyAbilityTagsToGameplayEffectSpec(Spec, AbilitySpec);

	if (const FHitResult* HitResult = Spec.GetContext().GetHitResult())
	{
		// TODO: Physical material with tags
	}
}

bool UGameplayAbilityV2::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags,
                                                           const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bBlocked(false), bMissing(false);

	const UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();
	const FGameplayTag& BlockedTag = AbilitySystemGlobals.ActivateFailTagsBlockedTag;
	const FGameplayTag& MissingTag = AbilitySystemGlobals.ActivateFailTagsMissingTag;

	// Check if any of this ability's tags are currently blocked.
	bBlocked = AbilitySystemComponent.AreAbilityTagsBlocked(AbilityTags) ? true : false;

	static FGameplayTagContainer AllRequiredTags(ActivationRequiredTags), AllBlockedTags(ActivationBlockedTags);

	// Add additional required and blocked tags based on tag relationship mapping.
	if (const UAbilitySystemComponentV2* AbilitySystemComponentV2 = Cast<UAbilitySystemComponentV2>(&AbilitySystemComponent))
	{
		AbilitySystemComponentV2->GetAdditionalActivationTagRequirements(AbilityTags, AllRequiredTags, AllBlockedTags);
	}

	if (AllBlockedTags.Num() || AllRequiredTags.Num())
	{
		static FGameplayTagContainer AbilitySystemComponentTags;
		AbilitySystemComponentTags.Reset();
		AbilitySystemComponent.GetOwnedGameplayTags(AbilitySystemComponentTags);

		if (AbilitySystemComponentTags.HasAny(AllBlockedTags))
		{
			const FCoreGameplayTags& CoreGameplayTags = FCoreGameplayTags::Get();
			if (OptionalRelevantTags && AbilitySystemComponentTags.HasTag(CoreGameplayTags.Status_Death))
			{
				//T TODO: Ability_ActivateFail_IsDead
				// OptionalRelevantTags->AddTag(CoreGameplayTags.Ability_ActivateFail_IsDead);
			}
			bBlocked = true;
		}
		if (!AbilitySystemComponentTags.HasAll(AllRequiredTags))
		{
			bMissing = true;
		}
	}

	if (SourceTags)
	{
		if (SourceBlockedTags.Num() || SourceRequiredTags.Num())
		{
			if (SourceTags->HasAny(SourceBlockedTags))
			{
				bBlocked = true;
			}

			if (SourceTags->HasAll(SourceRequiredTags))
			{
				bMissing = true;
			}
		}
	}

	if (TargetTags)
	{
		if (TargetBlockedTags.Num() || TargetRequiredTags.Num())
		{
			if (TargetTags->HasAny(TargetBlockedTags))
			{
				bBlocked = true;
			}

			if (TargetTags->HasAll(TargetRequiredTags))
			{
				bMissing = true;
			}
		}
	}

	if (bBlocked)
	{
		if (OptionalRelevantTags && BlockedTag.IsValid())
		{
			OptionalRelevantTags->AddTag(BlockedTag);
		}

		return false;
	}

	if (bMissing)
	{
		if (OptionalRelevantTags && MissingTag.IsValid())
		{
			OptionalRelevantTags->AddTag(MissingTag);
		}

		return false;
	}

	return true;
	//return Super::DoesAbilitySatisfyTagRequirements(AbilitySystemComponent, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGameplayAbilityV2::OnPawnAvatarSet()
{
	Script_OnPawnAvatarSet();
}
