// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "AbilitySystemInterface.h"
#include "BartenderEnder/AbilitySystem/AbilitySystemComponentV2.h"
#include "GameFramework/Character.h"
#include "GameCharacter.generated.h"

class UHealthComponent;
class UFirstPersonCameraComponent;
class UPawnData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterReadyDelegate);

/**
 * Minimal base character class for game AI pawns and for the Player.
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "Minimal base character class for AI and Player."))
class AGameCharacter : public ABaseCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGameCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Returns enhanced Ability System Component.
	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual UAbilitySystemComponentV2* GetAbilitySystemComponentV2() const { return AbilitySystemComponent; }

	// Returns basic Ability System Component.
	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return GetAbilitySystemComponentV2(); }

	const UPawnData* GetPawnData() const { return PawnData; }
	void SetPawnData(const UPawnData* InPawnData, bool bCheckReadyToInitialize = true);

	void InitializeAbilitySystem(UAbilitySystemComponentV2* InAbilitySystemComponent, AActor* InOwnerActor);
	void UninitializeAbilitySystem();

	// Call to ensure character has valid PawnData and Controller for initialization.
	bool CheckReadyToInitialize();

	// Checks if character is ready to be initialized.
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Character", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsReadyToInitialize() const { return bReadyToInitialize; }

	// Register with OnReadyToInitialize character delegate and broadcast if necessary.
	void OnCharacterReadyToInitialize_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// Should be called after AbilitySystemComponent was successfully initialized to set up other dependent components.
	void OnAbilitySystemInitialized();

	// Should be called after AbilitySystemComponent was successfully uninitialized to clean up other dependent components.
	void OnAbilitySystemUninitialized();

	void InitializeGameplayTags();

	// Starting character death by disabling movement and collision.
	UFUNCTION()
	void OnDying(AActor* OwnerActor);

	// Ending character death by detaching controller and destroying pawn.
	UFUNCTION()
	void OnDeath(AActor* OwnerActor);

	// Blueprint event called when the death sequence for the character has completed.
	UFUNCTION(BlueprintImplementableEvent, Category = "Character", Meta = (DisplayName = "OnDeath"))
	void BP_OnDeath();

	void DisableMovementAndCollision() const;
	void HandleDeath();
	void UninitializeAndDestroy(bool bSetHidden = false, float RemainingLifeSpan = 5.f);

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);

protected:
	// Delegate fired when character has everything needed for initialization.
	FSimpleMulticastDelegate OnReadyToInitialize;

	UPROPERTY(BlueprintAssignable, Meta = (DisplayName = "On Ready To Initialize"))
	FCharacterReadyDelegate BP_OnReadyToInitialize;

private:
	// Pawn Data used for this character. Can be set by a spawn function or on a placed character instance.
	UPROPERTY(EditInstanceOnly, Category = "Pawn")
	const UPawnData* PawnData = nullptr;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", Meta = (AllowPrivateAccess = true))
	//UFirstPersonCameraComponent* CameraComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", Meta = (AllowPrivateAccess = true))
	UHealthComponent* HealthComponent = nullptr;

	// Cached AbilitySystemComponent pointer.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", Meta = (AllowPrivateAccess = true))
	UAbilitySystemComponentV2* AbilitySystemComponent = nullptr;

	uint8 bReadyToInitialize : 1;
};
