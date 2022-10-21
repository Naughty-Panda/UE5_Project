// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "GamePlayerController.generated.h"

class AGamePlayerState;
class UAbilitySystemComponentV2;

/**
 * Minimal controller class for the Player.
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "Minimal controller class for the Player."))
class AGamePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AGamePlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Returns Game Player State if valid.
	UFUNCTION(BlueprintCallable, Category = "Player Controller")
	AGamePlayerState* GetGamePlayerState() const;

	// Returns Ability System Component V2 if valid.
	UFUNCTION(BlueprintCallable, Category = "Player Controller")
	UAbilitySystemComponentV2* GetAbilitySystemComponentV2() const;

	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void ReceivedPlayer() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void OnUnPossess() override;

protected:
	virtual void SetPlayer(UPlayer* InPlayer) override;

	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	// TODO: User Settings
	void OnUserSettingsChanged(void* UserSettings) {}
};
