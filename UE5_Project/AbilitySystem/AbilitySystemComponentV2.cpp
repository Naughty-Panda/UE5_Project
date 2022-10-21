// Bartender Ender
// Naughty Panda @ 2022

#include "AbilitySystemComponentV2.h"
#include "NativeGameplayTags.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_AbilityInputBlocked, "Gameplay.AbilityInputBlocked");

UAbilitySystemComponentV2::UAbilitySystemComponentV2(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();

	FMemory::Memset(ActivationGroupCounts, 0, sizeof(ActivationGroupCounts));
}

void UAbilitySystemComponentV2::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// TODO: Unregister from Global Ability Subsystem.
	Super::EndPlay(EndPlayReason);
}

void UAbilitySystemComponentV2::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	check(ActorInfo);
	check(InOwnerActor);

	const bool bHasNewPawnAvatar = Cast<APawn>(InAvatarActor) && (InAvatarActor != ActorInfo->AvatarActor);

	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (!bHasNewPawnAvatar)
	{
		return;
	}

	// Notify all abilities that a new pawn avatar has been set.
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		UGameplayAbilityV2* AbilityCDO = CastChecked<UGameplayAbilityV2>(AbilitySpec.Ability);

		if (AbilityCDO->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
		{
			TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
			for (UGameplayAbility* AbilityInstance : Instances)
			{
				UGameplayAbilityV2* AbilityInstanceV2 = CastChecked<UGameplayAbilityV2>(AbilityInstance);
				AbilityInstanceV2->OnPawnAvatarSet();
			}
		}
		else
		{
			AbilityCDO->OnPawnAvatarSet();
		}
	}

	// TODO: init global ability subsystem
	// TODO: init anim instance

	TryActivateAbilitiesOnSpawn();
}

void UAbilitySystemComponentV2::CancelAbilitiesByFunction(TShouldCancelAbilityFunction Function)
{
	ABILITYLIST_SCOPE_LOCK();

	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (!AbilitySpec.IsActive())
		{
			continue;
		}

		UGameplayAbilityV2* AbilityCDO = CastChecked<UGameplayAbilityV2>(AbilitySpec.Ability);

		if (AbilityCDO->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
		{
			// Cancel all spawned instances.
			const TArray<UGameplayAbility*> AbilityInstances = AbilitySpec.GetAbilityInstances();
			for (UGameplayAbility* AbilityInstance : AbilityInstances)
			{
				UGameplayAbilityV2* AbilityV2Instance = CastChecked<UGameplayAbilityV2>(AbilityInstance);

				if (Function(AbilityV2Instance, AbilitySpec.Handle))
				{
					if (!AbilityV2Instance->CanBeCanceled())
					{
						AbilityV2Instance->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), AbilityV2Instance->GetCurrentActivationInfo(), false);
					}
					else
					{
						UE_LOG(LogAbilitySystem, Error, TEXT("CancelAbilitiesByFunction: Cannot cancel ability %s: CanBeCanceled = false!"), *AbilityV2Instance->GetName());
					}
				}
			}
		}
		else
		{
			// Cancel non-instanced ability CDO.
			if (Function(AbilityCDO, AbilitySpec.Handle))
			{
				// Non-instanced abilities can always be canceled.
				check(AbilityCDO->CanBeCanceled());
				AbilityCDO->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), FGameplayAbilityActivationInfo(), false);
			}
		}
	}
}

void UAbilitySystemComponentV2::CancelInputActivatedAbilities()
{
	TShouldCancelAbilityFunction Function = [this](const UGameplayAbilityV2* Ability, FGameplayAbilitySpecHandle SpecHandle) -> bool
	{
		const EGameplayAbilityActivationPolicy ActivationPolicy = Ability->GetActivationPolicy();
		return ((ActivationPolicy == EGameplayAbilityActivationPolicy::OnInputTriggered) || (ActivationPolicy == EGameplayAbilityActivationPolicy::OnInputHeld));
	};

	CancelAbilitiesByFunction(Function);
}

void UAbilitySystemComponentV2::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("AbilityInputTagPressed: Invalid Input Tag!"));
		return;
	}

	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.Ability && AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
			InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
		}
	}
}

void UAbilitySystemComponentV2::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("AbilityInputTagReleased: Invalid Input Tag!"));
		return;
	}

	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.Ability && AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
			InputHeldSpecHandles.Remove(AbilitySpec.Handle);
		}
	}
}

void UAbilitySystemComponentV2::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	UE_LOG(LogAbilitySystem, Log, TEXT("ProcessAbilityInput!"));

	if (HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked))
	{
		ClearAbilityInput();
		return;
	}

	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	// Process all abilities that activate when input is held.
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const UGameplayAbilityV2* AbilityCDO = CastChecked<UGameplayAbilityV2>(AbilitySpec->Ability);

				if (AbilityCDO->GetActivationPolicy() == EGameplayAbilityActivationPolicy::OnInputHeld)
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	// Process abilities that had their input pressed this frame.
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				if (AbilitySpec->IsActive())
				{
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const UGameplayAbilityV2* AbilityCDO = CastChecked<UGameplayAbilityV2>(AbilitySpec->Ability);

					if (AbilityCDO->GetActivationPolicy() == EGameplayAbilityActivationPolicy::OnInputTriggered)
					{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	// Try to activate all Pressed and Hold abilities at once.
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle);
	}

	// Process abilities that had their input released this frame.
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				if (AbilitySpec->IsActive())
				{
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	// Clear cached ability handles.
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UAbilitySystemComponentV2::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

bool UAbilitySystemComponentV2::IsActivationGroupBlocked(EGameplayAbilityActivationGroup ActivationGroup) const
{
	bool bBlocked = false;

	switch (ActivationGroup)
	{
	case EGameplayAbilityActivationGroup::Independent:
		bBlocked = false;
		break;

	case EGameplayAbilityActivationGroup::Exclusive_Replaceable:
	case EGameplayAbilityActivationGroup::Exclusive_Blocking:
		bBlocked = ActivationGroupCounts[static_cast<uint8>(EGameplayAbilityActivationGroup::Exclusive_Blocking)] > 0;

	default:
		checkNoEntry();
		break;
	}

	return bBlocked;
}

void UAbilitySystemComponentV2::AddAbilityToActivationGroup(EGameplayAbilityActivationGroup Group, UGameplayAbilityV2* Ability)
{
	check(Ability);
	check(ActivationGroupCounts[static_cast<uint8>(Group)] < INT32_MAX);

	++ActivationGroupCounts[static_cast<uint8>(Group)];

	switch (Group)
	{
	case EGameplayAbilityActivationGroup::Independent:
		break;

	case EGameplayAbilityActivationGroup::Exclusive_Replaceable:
	case EGameplayAbilityActivationGroup::Exclusive_Blocking:
		CancelActivationGroupAbilities(EGameplayAbilityActivationGroup::Exclusive_Replaceable, Ability);
		break;

	default:
		checkNoEntry();
		break;
	}

	const int32 ExclusiveCount = ActivationGroupCounts[static_cast<uint8>(EGameplayAbilityActivationGroup::Exclusive_Replaceable)]
		+ ActivationGroupCounts[static_cast<uint8>(EGameplayAbilityActivationGroup::Exclusive_Blocking)];

	if (!ensure(ExclusiveCount <= 1))
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("AddAbilityToActivationGroup: multiple exclusive abilities detected!"));
	}
}

void UAbilitySystemComponentV2::RemoveAbilityFromActivationGroup(EGameplayAbilityActivationGroup Group, UGameplayAbilityV2* Ability)
{
	check(Ability);
	check(ActivationGroupCounts[static_cast<uint8>(Group)] > 0);

	--ActivationGroupCounts[static_cast<uint8>(Group)];
}

void UAbilitySystemComponentV2::CancelActivationGroupAbilities(EGameplayAbilityActivationGroup Group, UGameplayAbilityV2* IgnoredAbility)
{
	TShouldCancelAbilityFunction Function = [this, Group, IgnoredAbility](const UGameplayAbilityV2* Ability, FGameplayAbilitySpecHandle SpecHandle)-> bool
	{
		return ((Ability->GetActivationGroup() == Group) && (Ability != IgnoredAbility));
	};

	CancelAbilitiesByFunction(Function);
}

void UAbilitySystemComponentV2::AddDynamicTagGameplayEffect(const FGameplayTag& Tag)
{
	const TSubclassOf<UGameplayEffect> DynamicTagGameplayEffect;
	if (!DynamicTagGameplayEffect)
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("AddDynamicTagGameplayEffect: unable to find Gameplay Effect!"));
		return;
	}

	// TODO: Global Game Data.

	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(DynamicTagGameplayEffect, 1.f, MakeEffectContext());
	FGameplayEffectSpec* EffectSpec = SpecHandle.Data.Get();

	if (!EffectSpec)
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("AddDynamicTagGameplayEffect: Cannot make ongoing effect spec for %s!"), *GetNameSafe(DynamicTagGameplayEffect));
		return;
	}

	EffectSpec->DynamicGrantedTags.AddTag(Tag);

	ApplyGameplayEffectSpecToSelf(*EffectSpec);
}

void UAbilitySystemComponentV2::RemoveDynamicTagGameplayEffect(const FGameplayTag& Tag)
{
	const TSubclassOf<UGameplayEffect> DynamicTagGameplayEffect;
	if (!DynamicTagGameplayEffect)
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("RemoveDynamicTagGameplayEffect: unable to find Gameplay Effect!"));
		return;
	}

	// TODO: Global Game Data.

	FGameplayEffectQuery EffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(Tag));
	EffectQuery.EffectDefinition = DynamicTagGameplayEffect;

	RemoveActiveEffects(EffectQuery);
}

void UAbilitySystemComponentV2::GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired,
                                                                       FGameplayTagContainer& OutActivationBlocked) const
{
	if (true /*TagRelationshipMapping*/)
	{
		// TODO: Tag relationship mapping
	}
}

void UAbilitySystemComponentV2::TryActivateAbilitiesOnSpawn()
{
	ABILITYLIST_SCOPE_LOCK();

	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		const UGameplayAbilityV2* AbilityCDO = CastChecked<UGameplayAbilityV2>(AbilitySpec.Ability);
		AbilityCDO->TryActivateAbilityOnSpawn(AbilityActorInfo.Get(), AbilitySpec);
	}
}

void UAbilitySystemComponentV2::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	// UGameplayAbility::bReplicateInputDirectly is not supported, so we use replicated event.
	if (Spec.IsActive())
	{
		UE_LOG(LogAbilitySystem, Log, TEXT("%i"), Spec.InputID);
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UAbilitySystemComponentV2::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	// UGameplayAbility::bReplicateInputDirectly is not supported, so we use replicated event.
	if (Spec.IsActive())
	{
		UE_LOG(LogAbilitySystem, Log, TEXT("%i"), Spec.InputID);
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UAbilitySystemComponentV2::NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability)
{
	Super::NotifyAbilityActivated(Handle, Ability);

	UGameplayAbilityV2* AbilityV2 = CastChecked<UGameplayAbilityV2>(Ability);
	AddAbilityToActivationGroup(AbilityV2->GetActivationGroup(), AbilityV2);
}

void UAbilitySystemComponentV2::NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	Super::NotifyAbilityFailed(Handle, Ability, FailureReason);

	HandleAbilityFailed(Ability, FailureReason);
}

void UAbilitySystemComponentV2::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);

	UGameplayAbilityV2* AbilityV2 = CastChecked<UGameplayAbilityV2>(Ability);
	RemoveAbilityFromActivationGroup(AbilityV2->GetActivationGroup(), AbilityV2);
}

void UAbilitySystemComponentV2::ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags,
                                                               const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags)
{
	Super::ApplyAbilityBlockAndCancelTags(AbilityTags, RequestingAbility, bEnableBlockTags, BlockTags, bExecuteCancelTags, CancelTags);

	// TODO: Implement TagRelationship Mapping.
}

void UAbilitySystemComponentV2::HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bCanBeCanceled)
{
	Super::HandleChangeAbilityCanBeCanceled(AbilityTags, RequestingAbility, bCanBeCanceled);
}

void UAbilitySystemComponentV2::HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason) const
{
	UE_LOG(LogAbilitySystem, Warning, TEXT("Ability %s failed to activate!"), *GetPathNameSafe(Ability));

	const UGameplayAbilityV2* AbilityV2 = CastChecked<UGameplayAbilityV2>(Ability);
	AbilityV2->OnAbilityFailedToActivate(FailureReason);
}
