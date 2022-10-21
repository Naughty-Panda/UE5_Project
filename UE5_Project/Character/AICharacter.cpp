// Bartender Ender
// Naughty Panda @ 2022

#include "AICharacter.h"
#include "BartenderEnder/Player/BaseAIController.h"

AAICharacter::AAICharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AIControllerClass = ABaseAIController::StaticClass();
}

void AAICharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}
