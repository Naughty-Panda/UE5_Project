// Bartender Ender
// Naughty Panda @ 2022

#include "GamePlayerState.h"
#include "AbilitySystemComponent.h"
#include "BartenderEnder/LogCategories.h"
#include "BartenderEnder/AbilitySystem/AbilitySet.h"
#include "BartenderEnder/AbilitySystem/Attributes/CombatSet.h"
#include "BartenderEnder/AbilitySystem/Attributes/HealthSet.h"
#include "BartenderEnder/Character/GameCharacter.h"
#include "BartenderEnder/GameModes/BEGameModeBase.h"
#include "Components/PlayerStateComponent.h"
#include "Components/GameFrameworkComponentManager.h"

AGamePlayerState::AGamePlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponentV2>(TEXT("AbilitySystemComponent"));

	CreateDefaultSubobject<UHealthSet>(TEXT("HealthSet"));
	CreateDefaultSubobject<UCombatSet>(TEXT("CombatSet"));
}

void AGamePlayerState::SetPawnData(const UPawnData* InPawnData)
{
	check(InPawnData);

	if (PawnData)
	{
		UE_LOG(LogPlayerState, Error, TEXT("PlayerState %s already has valid PawnData"), *GetNameSafe(this));
		return;
	}

	PawnData = InPawnData;

	for (const UAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (!AbilitySet)
		{
			UE_LOG(LogPlayerState, Error, TEXT("Invalid Ability Set on Player State %s"), *GetNameSafe(this));
			continue;
		}

		AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
	}

	// TODO: Send Game Framework Component Extension Event?
}

void AGamePlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AGamePlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!AbilitySystemComponent)
	{
		UE_LOG(LogPlayerState, Error, TEXT("AGamePlayerState::PostInitializeComponents(): AbilitySystemComponent = NULL!"));
		return;
	}

	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	const ABEGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ABEGameModeBase>();
	if (!GameMode)
	{
		UE_LOG(LogPlayerState, Error, TEXT("Cannot setup Pawn Data for the PlayerState: GameMode is not a subclass of ABEGameMode!"));
		return;
	}

	const UPawnData* GameModePawnData = GameMode->GetPawnDataForController(GetOwningController());
	if (!GameModePawnData)
	{
		UE_LOG(LogPlayerState, Error, TEXT("Cannot get a valid Pawn Data for the owning controller in GamePlayerState!"));
		return;
	}

	SetPawnData(GameModePawnData);
}

void AGamePlayerState::ClientInitialize(AController* C)
{
	Super::ClientInitialize(C);

	if (AGameCharacter* Pawn = GetPawn<AGameCharacter>())
	{
		Pawn->CheckReadyToInitialize();
	}

	UE_LOG(LogPlayerState, Warning, TEXT("AGamePlayerState::ClientInitialize()..."));
}

void AGamePlayerState::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);
	Super::BeginPlay();
}

void AGamePlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);
	Super::EndPlay(EndPlayReason);
}

void AGamePlayerState::Reset()
{
	Super::Reset();

	TArray<UPlayerStateComponent*> PlayerStateComponents;
	GetComponents(PlayerStateComponents);

	for (UPlayerStateComponent* Component : PlayerStateComponents)
	{
		Component->Reset();
	}
}
