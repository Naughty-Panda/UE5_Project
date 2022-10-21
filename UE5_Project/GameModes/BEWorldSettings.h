// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "BEWorldSettings.generated.h"

class UBEGameDefinition;

/**
 * Extended World Settings to support Game Definition asset.
 */
UCLASS()
class BARTENDERENDER_API ABEWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:
	ABEWorldSettings(const FObjectInitializer& ObjectInitializer);

public:
	FPrimaryAssetId GetDefaultGameDefinition() const;

protected:
	// The default Game Definition to use with this level.
	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	TSoftClassPtr<UBEGameDefinition> DefaultGameDefinition;
};
