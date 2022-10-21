// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "IInteractable.generated.h"

class UUserWidget;
class IInteractable;
class UAbilitySystemComponent;
struct FGameplayEventData;

/**
 * Structure that describes a single interaction option.
 */
USTRUCT(BlueprintType)
struct FInteractionOption
{
	GENERATED_BODY()

public:
	// Interactable Target.
	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	TScriptInterface<IInteractable> InteractableTarget;

	// Interaction text.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText InteractionText;

	// Interaction sub-text.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText InteractionSubText;

	// The ability to grant the avatar when they get near interactable object.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	TSubclassOf<UGameplayAbility> InteractionAbilityToGrant;

	// Target ability system component that can be interacted with.
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystem;

	// The ability spec to activate on the target ability system.
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	FGameplayAbilitySpecHandle TargetInteractionAbilityHandle;

	// Widget class to display for this interaction.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	TSoftClassPtr<UUserWidget> InteractionWidgetClass;

public:
	FORCEINLINE bool operator == (const FInteractionOption& Other) const
	{
		return InteractableTarget == Other.InteractableTarget
		&& InteractionAbilityToGrant == Other.InteractionAbilityToGrant
		&& TargetAbilitySystem == Other.TargetAbilitySystem
		&& TargetInteractionAbilityHandle == Other.TargetInteractionAbilityHandle
		&& InteractionWidgetClass == Other.InteractionWidgetClass
		&& InteractionText.IdenticalTo(Other.InteractionText)
		&& InteractionSubText.IdenticalTo(Other.InteractionSubText);
	}

	FORCEINLINE bool operator != (const FInteractionOption& Other) const
	{
		return !operator==(Other);
	}

	FORCEINLINE bool operator < (const FInteractionOption& Other) const
	{
		return InteractableTarget.GetInterface() < Other.InteractableTarget.GetInterface();
	}
};

/**
 * Interaction Query.
 */
USTRUCT(BlueprintType)
struct FInteractionQuery
{
	GENERATED_BODY()

public:
	// Requesting actor.
	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	TWeakObjectPtr<AActor> RequestingActor;

	// Requesting controller.
	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	TWeakObjectPtr<AController> RequestingController;

	// UObject for optional interaction data.
	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	TWeakObjectPtr<UObject> OptionalObjectData;
};

/**
 * Interaction Option Builder.
 */
struct FInteractionOptionBuilder
{
public:
	FInteractionOptionBuilder(TScriptInterface<IInteractable> InterfaceTargetScope, TArray<FInteractionOption>& InInteractionOptions)
		: InteractionScope(InterfaceTargetScope), InteractionOptions(InInteractionOptions)
	{
	}

	void AddInteractionOption(const FInteractionOption& Option)
	{
		FInteractionOption& NewOption = InteractionOptions.Add_GetRef(Option);
		NewOption.InteractableTarget = InteractionScope;
	}

private:
	TScriptInterface<IInteractable> InteractionScope;
	TArray<FInteractionOption>& InteractionOptions;
};



/**
 * UE Interface for things that can be interacted with.
 */
UINTERFACE(MinimalAPI, BlueprintType, Meta = (CannotImplementInterfaceInBlueprint))
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * C++ Interface for things that can be interacted with.
 */
class IInteractable
{
	GENERATED_BODY()

public:
	virtual void CollectInteractionOptions(const FInteractionQuery& InteractionQuery, FInteractionOptionBuilder& OptionBuilder) = 0;

	virtual void CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag, FGameplayEventData& EventData)
	{
	}
};



/**
 * Blueprint function library to interact with IInteractable interface.
 */
UCLASS()
class UInteractableStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Returns the Actor that implements provided Interactable interface.
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	static AActor* GetActorFromInteractableTarget(TScriptInterface<IInteractable> InteractableTarget);

	// Returns an array of Interactable interfaces if Actor or it's components implements them.
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	static void GetInteractableTargetsFromActor(AActor* Actor, TArray<TScriptInterface<IInteractable>>& OutInteractableTargetss);

	// Appends provided array of Interactable Targets with valid Overlap results.
	static void AppendInteractableTargetsFromOverlapResults(const TArray<FOverlapResult>& OverlapResults, TArray<TScriptInterface<IInteractable>>& OutInteractableTargets);

	// Appends provided array of Interactable Targets with valid Hit result.
	static void AppendInteractableTargetsFromHitResult(const FHitResult& HitResult,  TArray<TScriptInterface<IInteractable>>& OutInteractableTargets);

private:
	template <typename T>
	FORCEINLINE static void AddUniqueInteractableTargetsFrom(const T& Source, TArray<TScriptInterface<IInteractable>>& OutInteractableTargets)
	{
		if (const TScriptInterface<IInteractable> InteractableActor(Source.GetActor()); InteractableActor)
		{
			OutInteractableTargets.AddUnique(InteractableActor);
		}

		if (const TScriptInterface<IInteractable> InteractableComponent(Source.GetComponent()); InteractableComponent)
		{
			OutInteractableTargets.AddUnique(InteractableComponent);
		}
	}
};
