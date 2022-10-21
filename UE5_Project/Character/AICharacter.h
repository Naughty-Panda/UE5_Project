// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "GameCharacter.h"
#include "AICharacter.generated.h"

/**
 * Base character class for AI-controlled characters with Ability System Component.
 */
UCLASS(Blueprintable, Meta = (ShortTooltip = "Base class for AI-controlled characters with Ability System support."))
class AAICharacter : public AGameCharacter
{
	GENERATED_BODY()

public:
	AAICharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
};
