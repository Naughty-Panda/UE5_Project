// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "BartenderEnder/Input/MappableConfigContainer.h"
#include "Components/PawnComponent.h"
#include "PlayerPawnComponent.generated.h"

struct FInputActionValue;
class UPlayerInputComponent;
class UEnhancedInputLocalPlayerSubsystem;

/**
 * Player Pawn Component allows pawns to receive player input.
 * Should be added to any player-controlled character.
 */
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class UPlayerPawnComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UPlayerPawnComponent(const FObjectInitializer& ObjectInitializer);

	// Tries to find Player Pawn Component for the specified actor.
	UFUNCTION(BlueprintPure, Category = "Player")
	static UPlayerPawnComponent* FindPlayerPawnComponent(const AActor* Actor);

	bool HasPawnInitialized() const { return bPawnHasInitialized; }
	bool IsReadyToBindInputs() const { return bReadyToBindInputs; }

	// Tries to update cached fields (ControlledPawn, Controller).
	FORCEINLINE void UpdateCachedData();

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void InitializePlayerInput(UInputComponent* InputComponent);

	// Helper method to get InputComponent and InputSubsystem.
	TPair<UPlayerInputComponent*, UEnhancedInputLocalPlayerSubsystem*> GetInputComponentAndSubsystem() const;

	// Binds to OnInputConfigActivated GameSettings delegate.
	void OnInputConfigActivated(const FLoadedMappableConfigContainer& ConfigContainer) const;

	// Binds to OnInputConfigDeactivated GameSettings delegate.
	void OnInputConfigDeactivated(const FLoadedMappableConfigContainer& ConfigContainer) const;

	virtual bool IsPawnComponentReadyToInitialize() const;
	void OnPawnReadyToInitialize();

	// Notifies Ability System Component about pressed Input Tag.
	void Input_AbilityInputTagPressed(FGameplayTag InputTag) const;

	// Notifies Ability System Component about released Input Tag.
	void Input_AbilityInputTagReleased(FGameplayTag InputTag) const;

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_LookGamepad(const FInputActionValue& InputActionValue);

protected:
	// Input configs to be added to the player.
	UPROPERTY(EditAnywhere)
	TArray<FMappableConfigContainer> DefaultInputConfigs;

	// TODO: Camera modes.

	// If Pawn has finished it's initialization.
	uint8 bPawnHasInitialized : 1;

	// If Player input bindings have been applied.
	uint8 bReadyToBindInputs : 1;

	// Cached pointer to a controlled pawn.
	UPROPERTY()
	APawn* ControlledPawn = nullptr;

	// Cached pointer to a controlled pawn's controller.
	UPROPERTY()
	AController* Controller = nullptr;
};
