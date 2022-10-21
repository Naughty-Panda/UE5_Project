// Bartender Ender
// Naughty Panda @ 2022

#include "GameCharacter.h"
#include "AbilitySystemComponent.h"
#include "BartenderEnder/CoreGameplayTags.h"
#include "BartenderEnder/LogCategories.h"
#include "BartenderEnder/Components/HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AGameCharacter::AGameCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReadyToInitialize = false;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnDying.AddDynamic(this, &ThisClass::OnDying);
	HealthComponent->OnDeath.AddDynamic(this, &ThisClass::OnDeath);

	// TODO: other constructor stuff.
}

void AGameCharacter::SetPawnData(const UPawnData* InPawnData, bool bCheckReadyToInitialize)
{
	if (!InPawnData)
	{
		UE_LOG(LogCharacter, Error, TEXT("Cannot set Pawn Data: PawnData = NULL."));
		return;
	}

	bReadyToInitialize = false;

	if (PawnData)
	{
		UE_LOG(LogCharacter, Error, TEXT("This character already has a valid PawnData!"));
		return;
	}

	PawnData = InPawnData;

	if (bCheckReadyToInitialize)
	{
		CheckReadyToInitialize();
	}
}

void AGameCharacter::InitializeAbilitySystem(UAbilitySystemComponentV2* InAbilitySystemComponent, AActor* InOwnerActor)
{
	if (!InAbilitySystemComponent || !InOwnerActor)
	{
		UE_LOG(LogCharacter, Error, TEXT("Cannot initialize AbilitySystem: invalid AbilitySystemComponent or Owner!"));
		return;
	}

	if (AbilitySystemComponent == InAbilitySystemComponent)
	{
		UE_LOG(LogCharacter, Warning, TEXT("AbilitySystemComponent has been already initialized!"));
		return;
	}

	if (AbilitySystemComponent)
	{
		UninitializeAbilitySystem();
	}

	UE_LOG(LogCharacter, Warning, TEXT("Initializing new AbilitySystemComponent for %s!"), *GetNameSafe(this));

	AbilitySystemComponent = InAbilitySystemComponent;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, this);

	if (ensure(PawnData))
	{
		// TODO: SetTagRelationshipMappings for AbilitySystemComponent.
	}

	OnAbilitySystemInitialized();
}

void AGameCharacter::UninitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogCharacter, Error, TEXT("Cannot uninitialize AbilitySystem: AbilitySystemComponent = NULL!"));
		return;
	}

	if (AbilitySystemComponent->GetAvatarActor() == this)
	{
		AbilitySystemComponent->CancelAbilities(nullptr, nullptr);
		AbilitySystemComponent->ClearAbilityInput();
		AbilitySystemComponent->RemoveAllGameplayCues();

		if (AbilitySystemComponent->GetOwnerActor())
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			AbilitySystemComponent->ClearActorInfo();
		}

		OnAbilitySystemUninitialized();
	}

	AbilitySystemComponent = nullptr;
}

bool AGameCharacter::CheckReadyToInitialize()
{
	if (bReadyToInitialize)
	{
		return true;
	}

	if (!PawnData)
	{
		UE_LOG(LogCharacter, Error, TEXT("Character is not ready to initialize: Pawn Data is missing!"));
		return false;
	}

	if (!GetController())
	{
		UE_LOG(LogCharacter, Error, TEXT("Character is not ready to initialize: No Controller!"));
		return false;
	}

	// Here we can check if any components are ready for initialization.

	// Character is ready to initialize.
	bReadyToInitialize = true;
	OnReadyToInitialize.Broadcast();
	BP_OnReadyToInitialize.Broadcast();

	return true;
}

void AGameCharacter::OnCharacterReadyToInitialize_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnReadyToInitialize.IsBoundToObject(Delegate.GetUObject()))
	{
		OnReadyToInitialize.Add(Delegate);
	}

	if (bReadyToInitialize)
	{
		Delegate.Execute();
	}
}

void AGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AGameCharacter::OnAbilitySystemInitialized()
{
	if (!HealthComponent->TryInitializeWithAbilitySystem(AbilitySystemComponent))
	{
		UE_LOG(LogCharacter, Error, TEXT("HealthComponent: Error during Ability System initializatin!"));
		return;
	}

	InitializeGameplayTags();
}

void AGameCharacter::OnAbilitySystemUninitialized()
{
	HealthComponent->UninitializeFromAbilitySystem();
}

void AGameCharacter::InitializeGameplayTags()
{
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogCharacter, Error, TEXT("Cannot initialize gameplay tags: Ability System Component = NULL."));
		return;
	}

	const FCoreGameplayTags& CoreTags = FCoreGameplayTags::Get();

	for (const TPair<uint8, FGameplayTag>& TagMapping : CoreTags.MovementModeTagMap)
	{
		if (TagMapping.Value.IsValid())
		{
			AbilitySystemComponent->SetLooseGameplayTagCount(TagMapping.Value, 0);
		}
	}

	// TODO: Clear CustomMovementTags when we have them.

	const UCharacterMovementComponent* MovementComp = CastChecked<UCharacterMovementComponent>(GetMovementComponent());
	SetMovementModeTag(MovementComp->MovementMode, MovementComp->CustomMovementMode, true);
}

void AGameCharacter::OnDying(AActor* OwnerActor)
{
	DisableMovementAndCollision();
}

void AGameCharacter::OnDeath(AActor* OwnerActor)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleDeath);
}

void AGameCharacter::DisableMovementAndCollision() const
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	// TODO: Maybe we'll need collision later for ragdolls.
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);
	}

	if (UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent()))
	{
		MovementComponent->StopMovementImmediately();
		MovementComponent->DisableMovement();
	}
}

void AGameCharacter::HandleDeath()
{
	BP_OnDeath();
	UninitializeAndDestroy();
}

void AGameCharacter::UninitializeAndDestroy(bool bSetHidden, float RemainingLifeSpan)
{
	DetachFromControllerPendingDestroy();
	SetLifeSpan(RemainingLifeSpan);
	SetActorHiddenInGame(bSetHidden);

	// TODO: Uninitialize Ability System?
}

void AGameCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	const UCharacterMovementComponent* MovementComp = CastChecked<UCharacterMovementComponent>(GetMovementComponent());
	SetMovementModeTag(PrevMovementMode, PreviousCustomMode, false);
	SetMovementModeTag(MovementComp->MovementMode, MovementComp->CustomMovementMode, true);
}

void AGameCharacter::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)
{
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogCharacter, Error, TEXT("Cannot set movement mode tag: Ability System Component = NULL."));
		return;
	}

	const FCoreGameplayTags& CoreTags = FCoreGameplayTags::Get();
	const FGameplayTag* MovementModeTag = nullptr;

	if (MovementMode == MOVE_Custom)
	{
		// TODO: custom move modes.
		unimplemented();
	}
	else
	{
		MovementModeTag = CoreTags.MovementModeTagMap.Find(MovementMode);
	}

	if (MovementModeTag && MovementModeTag->IsValid())
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(*MovementModeTag, bTagEnabled ? 1 : 0);
	}
}
