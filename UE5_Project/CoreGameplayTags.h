// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * Core Gameplay Tags Singleton for Bartender Ender.
 */

struct FCoreGameplayTags
{
public:
	static const FCoreGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeTags();
	static FGameplayTag FindTagByString(FString TagString, bool bMatchPartialString = false);

protected:
	void AddAllTags(UGameplayTagsManager& TagManager);
	static void AddTag(UGameplayTagsManager& TagManager, FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);
	static void AddMovementModeTag(UGameplayTagsManager& TagManager, FGameplayTag& OutTag, const ANSICHAR* TagName, uint8 MovementMode);

private:
	static FCoreGameplayTags GameplayTags;

public:
	// Input Tags
	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_Run;
	FGameplayTag InputTag_Crouch;
	FGameplayTag InputTag_Look_Mouse;
	FGameplayTag InputTag_Look_Gamepad;

	// Input Ability Tags
	FGameplayTag InputTag_Jump;
	FGameplayTag InputTag_Ability_Interact;
	FGameplayTag InputTag_Throw;

	// Equipment Slots Input
	FGameplayTag InputTag_Slots_PrimaryNext;
	FGameplayTag InputTag_Slots_PrimaryPrevious;
	FGameplayTag InputTag_Slots_PrimaryDrop;

	FGameplayTag InputTag_Slots_SecondaryNext;
	FGameplayTag InputTag_Slots_SecondaryPrevious;
	FGameplayTag InputTag_Slots_SecondaryDrop;

	// Status Tags
	FGameplayTag Status_Running;
	FGameplayTag Status_Crouching;
	FGameplayTag Status_Death;
	FGameplayTag Status_Death_Dying;
	FGameplayTag Status_Death_Dead;

	// Gameplay Event Tags
	FGameplayTag GameplayEvent_Death;

	// Movement Mode Tags
	FGameplayTag Movement_Mode_Walking;
	FGameplayTag Movement_Mode_NavWalking;
	FGameplayTag Movement_Mode_Falling;
	FGameplayTag Movement_Mode_Swimming;
	FGameplayTag Movement_Mode_Flying;
	FGameplayTag Movement_Mode_Custom;

	TMap<uint8, FGameplayTag> MovementModeTagMap;
};
