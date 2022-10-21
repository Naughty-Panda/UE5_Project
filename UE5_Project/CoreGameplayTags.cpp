// Bartender Ender
// Naughty Panda @ 2022

#include "CoreGameplayTags.h"
#include "GameplayTagsManager.h"

FCoreGameplayTags FCoreGameplayTags::GameplayTags;

void FCoreGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();
	GameplayTags.AddAllTags(TagsManager);

	// Notify manager when we are done adding native tags.
	TagsManager.DoneAddingNativeTags();
}

FGameplayTag FCoreGameplayTags::FindTagByString(FString TagString, bool bMatchPartialString)
{
	const UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();
	FGameplayTag RequestedTag = TagsManager.RequestGameplayTag(FName(*TagString), false);

	if (!RequestedTag.IsValid() && bMatchPartialString)
	{
		FGameplayTagContainer AllTags;
		TagsManager.RequestAllGameplayTags(AllTags, true);

		for (const auto& Tag : AllTags)
		{
			if (Tag.ToString().Contains(TagString))
			{
				UE_LOG(LogGameplayTags, Display, TEXT("Exact match for Tag %s was not found, partial match: %s"), *TagString, *Tag.ToString());
				RequestedTag = Tag;
				break;
			}
		}
	}

	return RequestedTag;
}

void FCoreGameplayTags::AddAllTags(UGameplayTagsManager& TagManager)
{
	AddTag(TagManager, InputTag_Move, "InputTag.Move", "Move input.");
	AddTag(TagManager, InputTag_Run, "InputTag.Run", "Run input.");
	AddTag(TagManager, InputTag_Crouch, "InputTag.Crouch", "Crouch input.");
	AddTag(TagManager, InputTag_Look_Mouse, "InputTag.Look.Mouse", "Mouse look input.");
	AddTag(TagManager, InputTag_Look_Gamepad, "InputTag.Look.Gamepad", "Gamepad look input.");

	AddTag(TagManager, InputTag_Jump, "InputTag.Jump", "Jump input.");
	AddTag(TagManager, InputTag_Ability_Interact, "InputTag.Ability.Interact", "Interaction ability input.");
	AddTag(TagManager, InputTag_Throw, "InputTag.Throw", "Input for throwing items.");

	AddTag(TagManager, InputTag_Slots_PrimaryNext, "InputTag.EquipmentSlots.PrimaryNext", "Select next primary slot.");
	AddTag(TagManager, InputTag_Slots_PrimaryPrevious, "InputTag.EquipmentSlots.PrimaryPrevious", "Select previous primary slot.");
	AddTag(TagManager, InputTag_Slots_PrimaryDrop, "InputTag.EquipmentSlots.PrimaryDrop", "Drop item from primary slot.");

	AddTag(TagManager, InputTag_Slots_SecondaryNext, "InputTag.EquipmentSlots.SecondaryNext", "Select next secondary slot.");
	AddTag(TagManager, InputTag_Slots_SecondaryPrevious, "InputTag.EquipmentSlots.SecondaryPrevious", "Select previous secondary slot.");
	AddTag(TagManager, InputTag_Slots_SecondaryDrop, "InputTag.EquipmentSlots.SecondaryDrop", "Drop item from secondary slot.");

	AddTag(TagManager, Status_Running, "Status.Running", "Target is running.");
	AddTag(TagManager, Status_Crouching, "Status.Crouching", "Target is crouching.");
	AddTag(TagManager, Status_Death, "Status.Death", "Target has death status.");
	AddTag(TagManager, Status_Death_Dying, "Status.Death.Dying", "Target is dying.");
	AddTag(TagManager, Status_Death_Dead, "Status.Death.Dead", "Target is dead.");

	AddTag(TagManager, GameplayEvent_Death, "GameplayEvent.Death", "Death event.");

	AddMovementModeTag(TagManager, Movement_Mode_Walking, "Movement.Mode.Walking", MOVE_Walking);
	AddMovementModeTag(TagManager, Movement_Mode_NavWalking, "Movement.Mode.NavWalking", MOVE_NavWalking);
	AddMovementModeTag(TagManager, Movement_Mode_Falling, "Movement.Mode.Falling", MOVE_Falling);
	AddMovementModeTag(TagManager, Movement_Mode_Swimming, "Movement.Mode.Swimming", MOVE_Swimming);
	AddMovementModeTag(TagManager, Movement_Mode_Flying, "Movement.Mode.Flying", MOVE_Flying);
	AddMovementModeTag(TagManager, Movement_Mode_Custom, "Movement.Mode.Custom", MOVE_Custom);
}

void FCoreGameplayTags::AddTag(UGameplayTagsManager& TagManager, FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = TagManager.AddNativeGameplayTag(FName(TagName), FString(TEXT("(Native) ") + FString(TagComment)));
}

void FCoreGameplayTags::AddMovementModeTag(UGameplayTagsManager& TagManager, FGameplayTag& OutTag, const ANSICHAR* TagName, uint8 MovementMode)
{
	AddTag(TagManager, OutTag, TagName, "Character movement mode tag.");
	GameplayTags.MovementModeTagMap.Add(MovementMode, OutTag);
}
