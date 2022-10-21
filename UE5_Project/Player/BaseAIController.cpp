// Bartender Ender
// Naughty Panda @ 2022

#include "BaseAIController.h"
#include "GamePlayerState.h"
#include "BartenderEnder/Character/AICharacter.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/GameStateBase.h"

ABaseAIController::ABaseAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsPlayerState = true;
}

UAbilitySystemComponent* ABaseAIController::GetAbilitySystemComponent() const
{
	const AGamePlayerState* AIPlayerState = GetPlayerState<AGamePlayerState>();
	return AIPlayerState ? AIPlayerState->GetAbilitySystemComponent() : nullptr;
}

void ABaseAIController::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void ABaseAIController::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);
	Super::BeginPlay();
}

void ABaseAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);
	Super::EndPlay(EndPlayReason);
}

void ABaseAIController::InitPlayerState()
{
	UWorld* const World = GetWorld();
	const AGameModeBase* GameMode = World ? World->GetAuthGameMode() : nullptr;

	if (!GameMode)
	{
		const AGameStateBase* const GameState = World ? World->GetGameState() : nullptr;
		GameMode = GameState ? GameState->GetDefaultGameMode() : nullptr;
	}

	if (!GameMode)
	{
		UE_LOG(LogPlayerController, Error, TEXT("%s cannot init AIPlayerState: no valid GemaMode!"), *GetNameSafe(this));
		return;
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = this;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// We never want player states to save into a map
	SpawnInfo.ObjectFlags |= RF_Transient;

	PlayerState = World->SpawnActor<AGamePlayerState>(SpawnInfo);

	// Force a default AI name if necessary
	if (PlayerState && PlayerState->GetPlayerName().IsEmpty())
	{
		PlayerState->SetPlayerNameInternal(GameMode->DefaultPlayerName.ToString());
	}
}

void ABaseAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AAICharacter* AIPawn = Cast<AAICharacter>(InPawn);
	if (!AIPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("%s is not AICharacter!"), *GetNameSafe(InPawn));
		return;
	}

	AGamePlayerState* GamePlayerState = GetPlayerState<AGamePlayerState>();
	if (!GamePlayerState)
	{
		UE_LOG(LogTemp, Error, TEXT("%s cannot get AGamePlayerState!"), * GetNameSafe(this));
		return;
	}

	const UPawnData* GameStatePawnData = GamePlayerState->GetPawnData();
	if (!GameStatePawnData)
	{
		UE_LOG(LogTemp, Error, TEXT("%s cannot get valid Pawn Data!"), *GetNameSafe(InPawn));
		return;
	}

	AIPawn->SetPawnData(GameStatePawnData, false);
	AIPawn->InitializeAbilitySystem(GamePlayerState->GetAbilitySystemComponentV2(), GamePlayerState);
	AIPawn->CheckReadyToInitialize();
}

void ABaseAIController::OnUnPossess()
{
	if (AAICharacter* AIPawn = GetPawn<AAICharacter>())
	{
		AIPawn->UninitializeAbilitySystem();
	}

	Super::OnUnPossess();
}
