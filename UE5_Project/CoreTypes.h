// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "CoreTypes.generated.h"

/**
 * Custom Trace Channels.
 */

// Trace used by character to detect climbable objects.
#define TraceChannel_Climbing ECC_GameTraceChannel1

// Trace used by character to detect interactions.
#define TraceChannel_Interaction ECC_GameTraceChannel2

/**
 * Core Game Types.
 */

UENUM(BlueprintType)
enum class EDeathState : uint8
{
	NotDead,
	Dying,
	Dead
};

UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	None,
	Primary,
	Secondary
};

UENUM(BlueprintType)
enum class EParkourPlayerState : uint8
{
	Run,
	Walk,
	Crouch,
	Crawl,
	Slide,
	Slideover
};

UENUM(BlueprintType)
enum class EParkourPlayerLean : uint8
{
	Straight,
	LeanLeft,
	LeanRight
};
