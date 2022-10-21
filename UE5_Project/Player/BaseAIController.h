// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"
#include "BaseAIController.generated.h"

/**
 * Base AI Controller class with Ability System support.
 */
UCLASS(Blueprintable)
class ABaseAIController : public AAIController, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABaseAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Initializes AGamePlayerState instead of the default one.
	// TODO: Create AIPlayerState to use AI Pawn Data?
	virtual void InitPlayerState() override;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
};
