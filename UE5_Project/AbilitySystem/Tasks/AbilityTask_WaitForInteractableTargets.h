// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "BartenderEnder/Interface/IInteractable.h"
#include "AbilityTask_WaitForInteractableTargets.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableObjectsChangedEvent, const TArray<FInteractionOption>&, InteractionOption);

/**
 * Ability Task to wait for IInteractable targets.
 */
UCLASS(Abstract)
class UAbilityTask_WaitForInteractableTargets : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FInteractableObjectsChangedEvent OnInteractableObjectsChanged;

protected:
	FORCEINLINE static void LineTrace(OUT FHitResult& OutHit, const TObjectPtr<UWorld> World, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams& QueryParams);
	FORCEINLINE static bool ClipCameraRayToAbilityRange(const FVector& CameraLocation, const FVector& CameraDirection, const FVector& AbilityCenter, float AbilityRange, FVector& ClippedPosition);
	void AimWithPlayerController(const TObjectPtr<AActor> Actor, const FVector& Start, FVector& End, float MaxRange, const FCollisionQueryParams& QueryParams, bool bIgnorePitch = false) const;
	void UpdateInteractionOptions(const FInteractionQuery& InteractionQuery, const TArray<TScriptInterface<IInteractable>>& InteractableTargets);

	FCollisionProfileName TraceProfile;
	bool bTraceAffectsAimPitch = true;
	TArray<FInteractionOption> InteractionOptions;
};


/**
 * Ability Task to wait for a trace for Interactable Targets.
 */
UCLASS()
class UAbilityTask_WaitForInteractableTargets_SingleLineTrace : public UAbilityTask_WaitForInteractableTargets
{
	GENERATED_BODY()

public:
	virtual void Activate() override;

	// Wait until a new set of interactable objects will be traced.
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", Meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = true))
	static UAbilityTask_WaitForInteractableTargets_SingleLineTrace* WaitForInteractableTargets_SingleLineTrace(UGameplayAbility* OwningAbility, const FInteractionQuery& InteractionQuery,
	                                                                                                           FCollisionProfileName TraceProfile,
	                                                                                                           const FGameplayAbilityTargetingLocationInfo& StartLocation,
	                                                                                                           float InteractionScanRate = .1f, float InteractionScanRange = 100.f,
	                                                                                                           bool bShowDebug = false);

private:
	virtual void OnDestroy(bool bInOwnerFinished) override;
	void ExecuteTrace();

private:
	UPROPERTY()
	FInteractionQuery InteractionQuery;

	// Trace starting location.
	UPROPERTY()
	FGameplayAbilityTargetingLocationInfo StartLocation;

	// Trace Timer Handle.
	FTimerHandle TimerHandle;

	float InteractionScanRate = .1f;
	float InteractionScanRange = 100.f;
	bool bShowDebug = false;
};
