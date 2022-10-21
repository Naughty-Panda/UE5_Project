// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

/**
 * Base Character class with UE5 Game Features and Modular Gameplay support.
 */

UCLASS(Blueprintable)
class BARTENDERENDER_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
