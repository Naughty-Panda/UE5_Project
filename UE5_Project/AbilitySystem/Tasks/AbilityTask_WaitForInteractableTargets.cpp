// Bartender Ender
// Naughty Panda @ 2022

#include "AbilityTask_WaitForInteractableTargets.h"
#include "AbilitySystemComponent.h"

void UAbilityTask_WaitForInteractableTargets::LineTrace(FHitResult& OutHit, const TObjectPtr<UWorld> World, const FVector& Start,
                                                        const FVector& End, FName ProfileName, const FCollisionQueryParams& QueryParams)
{
	check(World);

	OutHit = FHitResult();
	TArray<FHitResult> HitResults;
	World->LineTraceMultiByProfile(HitResults, Start, End, ProfileName, QueryParams);

	OutHit.TraceStart = Start;
	OutHit.TraceEnd = End;

	if (HitResults.Num() > 0)
	{
		OutHit = HitResults[0];
	}
}

bool UAbilityTask_WaitForInteractableTargets::ClipCameraRayToAbilityRange(const FVector& CameraLocation, const FVector& CameraDirection,
                                                                          const FVector& AbilityCenter, float AbilityRange, FVector& ClippedPosition)
{
	const FVector DistanceFromCameraToAbilityCenter = AbilityCenter - CameraLocation;
	const float DotToAbilityCenter = FVector::DotProduct(DistanceFromCameraToAbilityCenter, CameraDirection);
	if (DotToAbilityCenter >= 0)
	{
		const float DistanceSquared = DistanceFromCameraToAbilityCenter.SizeSquared() - (DotToAbilityCenter * DotToAbilityCenter);
		const float RadiusSquared = (AbilityRange * AbilityRange);
		if (DistanceSquared <= RadiusSquared)
		{
			const float DistanceFromCamera = FMath::Sqrt(RadiusSquared - DistanceSquared);
			const float DistanceAlongRay = DotToAbilityCenter + DistanceFromCamera;

			ClippedPosition = CameraLocation + (DistanceAlongRay * CameraDirection);
			return true;
		}
	}

	return false;
}

void UAbilityTask_WaitForInteractableTargets::AimWithPlayerController(const TObjectPtr<AActor> Actor, const FVector& Start, FVector& OutEnd, float MaxRange,
                                                                      const FCollisionQueryParams& QueryParams, bool bIgnorePitch) const
{
	if (!Ability)
	{
		return;
	}

	APlayerController* PlayerController = Ability->GetCurrentActorInfo()->PlayerController.Get();
	if (!PlayerController)
	{
		return;
	}

	FVector ViewStart;
	FRotator ViewRotation;
	PlayerController->GetPlayerViewPoint(ViewStart, ViewRotation);

	const FVector ViewDirection = ViewRotation.Vector();
	FVector ViewEnd = ViewStart + (ViewDirection * MaxRange);

	ClipCameraRayToAbilityRange(ViewStart, ViewDirection, Start, MaxRange, ViewEnd);

	FHitResult HitResult;
	LineTrace(HitResult, Actor->GetWorld(), ViewStart, ViewEnd, TraceProfile.Name, QueryParams);

	const bool bUseTraceResult = HitResult.bBlockingHit && (FVector::DistSquared(Start, HitResult.Location) <= (MaxRange * MaxRange));
	const FVector AdjustedEnd = bUseTraceResult ? HitResult.Location : ViewEnd;

	FVector AdjustedAimDirection = (AdjustedEnd - Start).GetSafeNormal();
	if (AdjustedAimDirection.IsZero())
	{
		AdjustedAimDirection = ViewDirection;
	}

	if (!bTraceAffectsAimPitch && bUseTraceResult)
	{
		FVector OriginalAimDirection = (ViewEnd - Start).GetSafeNormal();
		if (!OriginalAimDirection.IsZero())
		{
			const FRotator OriginalAimRotation = OriginalAimDirection.Rotation();

			FRotator AdjustedAimRotation = AdjustedAimDirection.Rotation();
			AdjustedAimRotation.Pitch = OriginalAimRotation.Pitch;

			AdjustedAimDirection = AdjustedAimRotation.Vector();
		}
	}

	OutEnd = Start + (AdjustedAimDirection * MaxRange);
}

void UAbilityTask_WaitForInteractableTargets::UpdateInteractionOptions(const FInteractionQuery& InteractionQuery, const TArray<TScriptInterface<IInteractable>>& InteractableTargets)
{
	TArray<FInteractionOption> NewInteractionOptions;

	for (const auto& InteractableTarget : InteractableTargets)
	{
		TArray<FInteractionOption> CurrentInteractionOptions;
		FInteractionOptionBuilder InteractionOptionBuilder(InteractableTarget, CurrentInteractionOptions);
		InteractableTarget->CollectInteractionOptions(InteractionQuery, InteractionOptionBuilder);

		for (auto& InteractionOption : CurrentInteractionOptions)
		{
			const FGameplayAbilitySpec* InteractionAbilitySpec = nullptr;

			// Check if target Ability System has a handle we can use.
			if (InteractionOption.TargetAbilitySystem && InteractionOption.TargetInteractionAbilityHandle.IsValid())
			{
				InteractionAbilitySpec = InteractionOption.TargetAbilitySystem->FindAbilitySpecFromHandle(InteractionOption.TargetInteractionAbilityHandle);
			}

			// TODO: Automatically grant ability to player?

			// Check if current options have interaction ability we can activate.
			else if (InteractionOption.InteractionAbilityToGrant)
			{
				InteractionAbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(InteractionOption.InteractionAbilityToGrant);

				if (InteractionAbilitySpec)
				{
					// Updating interaction option.
					InteractionOption.TargetAbilitySystem = AbilitySystemComponent;
					InteractionOption.TargetInteractionAbilityHandle = InteractionAbilitySpec->Handle;
				}
			}

			if (InteractionAbilitySpec)
			{
				// Add found ability if it can be activated.
				if (InteractionAbilitySpec->Ability->CanActivateAbility(InteractionAbilitySpec->Handle, AbilitySystemComponent->AbilityActorInfo.Get()))
				{
					NewInteractionOptions.Add(InteractionOption);
				}
			}
		}
	}

	bool bInteractionOptionsChanged = false;
	if (NewInteractionOptions.Num() == InteractionOptions.Num())
	{
		NewInteractionOptions.Sort();

		for (int32 OptionIndex = 0; OptionIndex < NewInteractionOptions.Num(); ++OptionIndex)
		{
			const FInteractionOption& NewInteractionOption = NewInteractionOptions[OptionIndex];
			const FInteractionOption& CurrentInteractionOption = InteractionOptions[OptionIndex];

			if (NewInteractionOption != CurrentInteractionOption)
			{
				bInteractionOptionsChanged = true;
				break;
			}
		}
	}
	else
	{
		bInteractionOptionsChanged = true;
	}

	if (bInteractionOptionsChanged)
	{
		InteractionOptions = NewInteractionOptions;
		OnInteractableObjectsChanged.Broadcast(InteractionOptions);
	}
}

void UAbilityTask_WaitForInteractableTargets_SingleLineTrace::Activate()
{
	SetWaitingOnAvatar();

	const UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("AbilityTask::Activate: No UWorld!"));
		return;
	}

	World->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::ExecuteTrace, InteractionScanRate, true);
}

UAbilityTask_WaitForInteractableTargets_SingleLineTrace* UAbilityTask_WaitForInteractableTargets_SingleLineTrace::WaitForInteractableTargets_SingleLineTrace(
	UGameplayAbility* OwningAbility, const FInteractionQuery& InteractionQuery, FCollisionProfileName TraceProfile,
	const FGameplayAbilityTargetingLocationInfo& StartLocation, float InteractionScanRate, float InteractionScanRange, bool bShowDebug)
{
	auto* AbilityTask = NewAbilityTask<UAbilityTask_WaitForInteractableTargets_SingleLineTrace>(OwningAbility);
	AbilityTask->InteractionQuery = InteractionQuery;
	AbilityTask->TraceProfile = TraceProfile;
	AbilityTask->StartLocation = StartLocation;
	AbilityTask->InteractionScanRate = InteractionScanRate;
	AbilityTask->InteractionScanRange = InteractionScanRange;
	AbilityTask->bShowDebug = bShowDebug;

	return AbilityTask;
}

void UAbilityTask_WaitForInteractableTargets_SingleLineTrace::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);

	const UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("AbilityTask::OnDestroy: No UWorld!"));
		return;
	}

	World->GetTimerManager().ClearTimer(TimerHandle);
}

void UAbilityTask_WaitForInteractableTargets_SingleLineTrace::ExecuteTrace()
{
	AActor* AvatarActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (!AvatarActor)
	{
		return;
	}

	constexpr bool bTraceComplex = false;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(UAbilityTask_WaitForInteractableTargets_SingleLineTrace), bTraceComplex);
	QueryParams.AddIgnoredActor(AvatarActor);

	const FVector TraceStart = StartLocation.GetTargetingTransform().GetLocation();
	FVector TraceEnd;
	AimWithPlayerController(AvatarActor, TraceStart, OUT TraceEnd, InteractionScanRange, QueryParams);

	UWorld* World = GetWorld();
	FHitResult OutHitResult;
	LineTrace(OutHitResult, World, TraceStart, TraceEnd, TraceProfile.Name, QueryParams);

	TArray<TScriptInterface<IInteractable>> InteractableTargets;
	UInteractableStatics::AppendInteractableTargetsFromHitResult(OutHitResult, InteractableTargets);

	UpdateInteractionOptions(InteractionQuery, InteractableTargets);

#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
		FColor DebugColor = OutHitResult.bBlockingHit ? FColor::Red : FColor::Green;
		if (OutHitResult.bBlockingHit)
		{
			DrawDebugLine(World, TraceStart, OutHitResult.Location, DebugColor, false, InteractionScanRate);
			DrawDebugSphere(World, OutHitResult.Location, 5.f, 16, DebugColor, false, InteractionScanRate);
		}
		else
		{
			DrawDebugLine(World, TraceStart, TraceEnd, DebugColor, false, InteractionScanRate);
		}
	}
#endif
}
