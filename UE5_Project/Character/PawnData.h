// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "PawnData.generated.h"

class UInputConfig;
class UAbilitySet;

/**
 * PawnData assets and settings used by GameCharacters.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Pawn Data", ShortTooltip = "Assets and settings used by GameCharacters"))
class UPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPawnData(const FObjectInitializer& ObjectInitializer);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn")
	TSubclassOf<APawn> PawnClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<UAbilitySet*> AbilitySets;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputConfig* InputConfig = nullptr;

	// TODO: Tag Relationship Mapping
	// TODO: Camera Mode
};
