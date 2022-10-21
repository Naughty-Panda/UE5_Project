// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "BEGameDefinition.generated.h"

class UPawnData;

/**
 * Default BE Game Definition.
 */

UCLASS(BlueprintType, Const)
class UBEGameDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UBEGameDefinition();

public:
	// Default Pawn Data to initialize player character.
	UPROPERTY(EditDefaultsOnly, Category = "Pawn Data")
	TSoftObjectPtr<const UPawnData> DefaultPawnData;
};
