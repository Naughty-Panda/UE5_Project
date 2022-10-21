// Bartender Ender
// Naughty Panda @ 2022

#include "BEGameModeBase.h"
#include "BEGameDefinition.h"
#include "BEWorldSettings.h"
#include "BartenderEnder/Character/GameCharacter.h"
#include "BartenderEnder/Character/PlayerCharacter.h"
#include "BartenderEnder/Player/GamePlayerController.h"
#include "BartenderEnder/Player/GamePlayerState.h"
#include "BartenderEnder/System/GameAssetManager.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/HUD.h"

ABEGameModeBase::ABEGameModeBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameSessionClass = AGameSession::StaticClass();
	GameStateClass = AGameStateBase::StaticClass();
	PlayerControllerClass = AGamePlayerController::StaticClass();
	PlayerStateClass = AGamePlayerState::StaticClass();
	HUDClass = AHUD::StaticClass();
	DefaultPawnClass = APlayerCharacter::StaticClass();
}

const UPawnData* ABEGameModeBase::GetPawnDataForController(const AController* Controller) const
{
	if (!Controller)
	{
		UE_LOG(LogGameMode, Error, TEXT("Cannot get Pawn Data: Controller = NULL!"));
		return nullptr;
	}

	// Check if we have PawnData on the GamePlayerState.
	if (const AGamePlayerState* PlayerState = Controller->GetPlayerState<AGamePlayerState>())
	{
		if (const UPawnData* PlayerStatePawnData = PlayerState->GetPawnData())
		{
			UE_LOG(LogGameMode, Warning, TEXT("Valid Pawn Data was found on the PlayerState."));
			return PlayerStatePawnData;
		}
	}

	// Try to get PawnData from Default Game Definition in world settings.
	if (const ABEWorldSettings* WorldSettings = Cast<ABEWorldSettings>(GetWorldSettings()))
	{
		const FPrimaryAssetId& GameDefinitionAssetID = WorldSettings->GetDefaultGameDefinition();

		if (GameDefinitionAssetID.IsValid())
		{
			if (FAssetData EmptyData; UAssetManager::Get().GetPrimaryAssetData(GameDefinitionAssetID, EmptyData))
			{
				const FSoftObjectPath GameDefinitionPath = UAssetManager::Get().GetPrimaryAssetPath(GameDefinitionAssetID);

				const TSubclassOf<UBEGameDefinition> GameDefinitionClass = Cast<UClass>(GameDefinitionPath.TryLoad());
				check(GameDefinitionClass);

				const UBEGameDefinition* GameDefinition = GetDefault<UBEGameDefinition>(GameDefinitionClass);
				check(GameDefinition);

				if (const UPawnData* GameDefinitionPawnData = GameDefinition->DefaultPawnData.Get())
				{
					UE_LOG(LogGameMode, Warning, TEXT("Valid Pawn Data was found in the Default GameDefinition."));
					return GameDefinitionPawnData;
				}
			}
		}
	}

	// Try to get Default PawnData from GameAssetManager.
	if (const UPawnData* AssetManagerPawnData = UGameAssetManager::Get().GetDefaultPawnData())
	{
		UE_LOG(LogGameMode, Warning, TEXT("Valid Default Pawn Data was provided by the GameAssetManager."));
		return AssetManagerPawnData;
	}

	UE_LOG(LogGameMode, Error, TEXT("No valid PawnData was found!"));
	return nullptr;
}

UClass* ABEGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const UPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			UE_LOG(LogGameMode, Log, TEXT("Default Pawn class was found on PawnData."));
			return PawnData->PawnClass;
		}
	}

	UE_LOG(LogGameMode, Warning, TEXT("Cannot get default Pawn class for controller: PawnData = NULL!"));
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

APawn* ABEGameModeBase::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Instigator = GetInstigator();
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.bDeferConstruction = true;

	UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer);
	if (!PawnClass)
	{
		UE_LOG(LogGameMode, Error, TEXT("Cannot spawn Player Pawn: DefaultPawnClass = NULL!"));
		return nullptr;
	}

	APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnParameters);
	if (!SpawnedPawn)
	{
		UE_LOG(LogGameMode, Error, TEXT("Cannot spawn Player Pawn class %s at %s"), *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
		return nullptr;
	}

	AGameCharacter* GameCharacter = Cast<AGameCharacter>(SpawnedPawn);
	if (!GameCharacter)
	{
		UE_LOG(LogGameMode, Warning, TEXT("Player Pawn %s was seccessfully spawned, but it's not AGameCharacter! Cannot initialize its PawnData!"), *GetNameSafe(SpawnedPawn));
		return FinishSpawning_Internal(SpawnedPawn, SpawnTransform);
	}

	const UPawnData* PawnData = GetPawnDataForController(NewPlayer);
	if (!PawnData)
	{
		UE_LOG(LogGameMode, Warning, TEXT("Player Pawn %s was seccessfully spawned, but PawnData for the controller was not found!"), *GetNameSafe(SpawnedPawn));
		return FinishSpawning_Internal(SpawnedPawn, SpawnTransform);
	}

	// Do not initialize character during this phase.
	// It may not be possessed by a controller yet.
	GameCharacter->SetPawnData(PawnData, false);
	return FinishSpawning_Internal(SpawnedPawn, SpawnTransform);
}

AActor* ABEGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	// TODO: Player start point.
	UE_LOG(LogGameMode, Warning, TEXT("ABEGameModeBase::ChoosePlayerStart_Implementation"));
	return Super::ChoosePlayerStart_Implementation(Player);
}

void ABEGameModeBase::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	UE_LOG(LogGameMode, Warning, TEXT("ABEGameModeBase::FinishRestartPlayer"));
	Super::FinishRestartPlayer(NewPlayer, StartRotation);
}

APawn* ABEGameModeBase::FinishSpawning_Internal(APawn* InPawn, const FTransform& SpawnTransform)
{
	check(InPawn);

	InPawn->FinishSpawning(SpawnTransform);
	return InPawn;
}
