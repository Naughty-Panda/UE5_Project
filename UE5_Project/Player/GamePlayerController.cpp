// Bartender Ender
// Naughty Panda @ 2022

#include "GamePlayerController.h"

#include "AbilitySystemGlobals.h"
#include "GamePlayerState.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Components/ControllerComponent.h"

AGamePlayerController::AGamePlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

AGamePlayerState* AGamePlayerController::GetGamePlayerState() const
{
	return CastChecked<AGamePlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UAbilitySystemComponentV2* AGamePlayerController::GetAbilitySystemComponentV2() const
{
	const AGamePlayerState* GamePlayerState = GetGamePlayerState();
	return GamePlayerState ? GamePlayerState->GetAbilitySystemComponentV2() : nullptr;
}

void AGamePlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AGamePlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetActorHiddenInGame(false);
}

void AGamePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);
	Super::EndPlay(EndPlayReason);
}

void AGamePlayerController::ReceivedPlayer()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);
	Super::ReceivedPlayer();

	TArray<UControllerComponent*> ControllerComponents;
	GetComponents(ControllerComponents);

	for (UControllerComponent* Component : ControllerComponents)
	{
		Component->ReceivedPlayer();
	}
}

void AGamePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// TODO: Uncomment when we have components.
	/*
	TArray<UControllerComponent*> ControllerComponents;
	GetComponents(ControllerComponents);

	for (UControllerComponent* Component : ControllerComponents)
	{
		Component->PlayerTick(DeltaTime);
	}
	*/
}

void AGamePlayerController::OnUnPossess()
{
	// Clear Ability System Avatar Actor if we unpossess it.
	if (const APawn* PossessedPawn = GetPawn())
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState))
		{
			if (AbilitySystemComponent->GetAvatarActor() == PossessedPawn)
			{
				UE_LOG(LogPlayerController, Warning, TEXT("Ability System Avatar cleared!"));
				AbilitySystemComponent->SetAvatarActor(nullptr);
			}
		}
	}

	Super::OnUnPossess();
}

void AGamePlayerController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);

	// TODO: User Settings.
	OnUserSettingsChanged(nullptr);
}

void AGamePlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PreProcessInput(DeltaTime, bGamePaused);
}

void AGamePlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UAbilitySystemComponentV2* AbilitySystemComponentV2 = GetAbilitySystemComponentV2())
	{
		AbilitySystemComponentV2->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}
