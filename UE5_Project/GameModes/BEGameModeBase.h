// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BEGameModeBase.generated.h"

class UPawnData;

/**
 * Base Game Mode class for Bartender Ender with minimal initial setup.
 */
UCLASS()
class ABEGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABEGameModeBase(const FObjectInitializer& ObjectInitializer);

	const UPawnData* GetPawnDataForController(const AController* Controller) const;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation) override;

private:
	FORCEINLINE APawn* FinishSpawning_Internal(APawn* InPawn, const FTransform& SpawnTransform);
};
