// Bartender Ender
// Naughty Panda @ 2022

#include "PlayerPawnComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BartenderEnder/CoreGameplayTags.h"
#include "BartenderEnder/LogCategories.h"
#include "BartenderEnder/Character/GameCharacter.h"
#include "BartenderEnder/Input/InputConfig.h"
#include "BartenderEnder/Input/PlayerInputComponent.h"
#include "BartenderEnder/Player/GamePlayerState.h"
#include "BartenderEnder/Settings/GameSettings.h"

UPlayerPawnComponent::UPlayerPawnComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), bPawnHasInitialized(false), bReadyToBindInputs(false)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

UPlayerPawnComponent* UPlayerPawnComponent::FindPlayerPawnComponent(const AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<UPlayerPawnComponent>() : nullptr;
}

void UPlayerPawnComponent::OnRegister()
{
	Super::OnRegister();

	AGameCharacter* Character = GetPawn<AGameCharacter>();

	if (!Character)
	{
		UE_LOG(LogPawnComponent, Error, TEXT("PlayerPawnComponent::OnRegister: Owner of this component is not AGameCharacter subclass. Cannot register component!"));
		return;
	}

	Character->OnCharacterReadyToInitialize_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnPawnReadyToInitialize));
}

void UPlayerPawnComponent::BeginPlay()
{
	Super::BeginPlay();

	UpdateCachedData();
}

void UPlayerPawnComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AGameCharacter* Character = GetPawn<AGameCharacter>())
	{
		Character->UninitializeAbilitySystem();
	}

	Super::EndPlay(EndPlayReason);
}

void UPlayerPawnComponent::InitializePlayerInput(UInputComponent* InputComponent)
{
	check(InputComponent);

	const AGameCharacter* Character = GetPawn<AGameCharacter>();

	if (!Character)
	{
		UE_LOG(LogPawnComponent, Error, TEXT("Cannot initialize player input: Owner of this component is not AGameCharacter subclass."));
		return;
	}

	const APlayerController* PlayerController = GetController<APlayerController>();
	check(PlayerController);

	const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	check(LocalPlayer);

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(InputSubsystem);

	InputSubsystem->ClearAllMappings();

	if (const UPawnData* PawnData = Character->GetPawnData())
	{
		if (const UInputConfig* InputConfig = PawnData->InputConfig)
		{
			const FCoreGameplayTags& GameplayTags = FCoreGameplayTags::Get();

			// Register any default configs with the settings.
			// They will be applied to the player during AddInputMappings.
			for (const FMappableConfigContainer& Container : DefaultInputConfigs)
			{
				if (Container.bShouldActivateAutomatically)
				{
					FMappableConfigContainer::ActivateContainer(Container);
				}
			}

			UPlayerInputComponent* PlayerInputComponent = CastChecked<UPlayerInputComponent>(InputComponent);
			PlayerInputComponent->AddInputMappings(InputConfig, InputSubsystem);

			// Bind delegates on input config changes.
			UGameSettings& GameSettings = UGameSettings::Get();
			GameSettings.OnInputConfigActivated.AddUObject(this, &ThisClass::OnInputConfigActivated);
			GameSettings.OnInputConfigDeactivated.AddUObject(this, &ThisClass::OnInputConfigDeactivated);

			// Bind Abilities.
			TArray<uint32> AbilityBindHandles;
			PlayerInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, AbilityBindHandles);

			// Bind Actions.
			PlayerInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
			PlayerInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse);
			PlayerInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Gamepad, ETriggerEvent::Triggered, this, &ThisClass::Input_LookGamepad);

			// TODO: other inputs.
		}
	}

	if (ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}

	// TODO: Send Game Framework Component Extension Event?
}

TPair<UPlayerInputComponent*, UEnhancedInputLocalPlayerSubsystem*> UPlayerPawnComponent::GetInputComponentAndSubsystem() const
{
	if (const APlayerController* PlayerController = GetController<APlayerController>())
	{
		if (const APawn* Pawn = GetPawn<APawn>())
		{
			if (UPlayerInputComponent* InputComponent = Cast<UPlayerInputComponent>(Pawn->InputComponent))
			{
				if (const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
				{
					if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
					{
						return {InputComponent, Subsystem};
					}
				}
			}
		}
	}

	return {nullptr, nullptr};
}

void UPlayerPawnComponent::OnInputConfigActivated(const FLoadedMappableConfigContainer& ConfigContainer) const
{
	const TPair<UPlayerInputComponent*, UEnhancedInputLocalPlayerSubsystem*> InputPair = GetInputComponentAndSubsystem();

	if (!InputPair.Key || !InputPair.Value)
	{
		UE_LOG(LogPawnComponent, Error, TEXT("Cannot add input config: Input Component or Subsystem = NULL!"));
		return;
	}

	InputPair.Key->AddInputConfig(ConfigContainer, InputPair.Value);
}

void UPlayerPawnComponent::OnInputConfigDeactivated(const FLoadedMappableConfigContainer& ConfigContainer) const
{
	const TPair<UPlayerInputComponent*, UEnhancedInputLocalPlayerSubsystem*> InputPair = GetInputComponentAndSubsystem();

	if (!InputPair.Key || !InputPair.Value)
	{
		UE_LOG(LogPawnComponent, Error, TEXT("Cannot add input config: Input Component or Subsystem = NULL!"));
		return;
	}

	InputPair.Key->RemoveInputConfig(ConfigContainer, InputPair.Value);
}

bool UPlayerPawnComponent::IsPawnComponentReadyToInitialize() const
{
	if (!GetPlayerState<AGamePlayerState>())
	{
		UE_LOG(LogPawnComponent, Error, TEXT("PlayerPawnComponent is not ready to initialize: PlayerState is not AGamePlayerState subclass!"));
		return false;
	}

	const APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		UE_LOG(LogPawnComponent, Error, TEXT("PlayerPawnComponent is not ready to initialize: Pawn = NULL!"));
		return false;
	}

	if (!Pawn->IsBotControlled() && !Pawn->InputComponent)
	{
		UE_LOG(LogPawnComponent, Error, TEXT("PlayerPawnComponent is not ready to initialize: Input component required for local player!"));
		return false;
	}

	return true;
}

void UPlayerPawnComponent::OnPawnReadyToInitialize()
{
	if (!ensure(!bPawnHasInitialized))
	{
		UE_LOG(LogPawnComponent, Error, TEXT("Pawn has already been initialized! No need to do it twice."));
		return;
	}

	AGameCharacter* Character = GetPawn<AGameCharacter>();

	if (!Character)
	{
		UE_LOG(LogPawnComponent, Error, TEXT("Owner of this component is not AGameCharacter subclass. Cannot initialize!"));
		return;
	}

	AGamePlayerState* PlayerState = GetPlayerState<AGamePlayerState>();

	if (!PlayerState)
	{
		UE_LOG(LogPawnComponent, Error, TEXT("Cannot initialize component: PlayerState is not AGamePlayerState subclass!"));
		return;
	}

	// Initialize Character's AbilitySystem from it's PlayerState.
	Character->InitializeAbilitySystem(PlayerState->GetAbilitySystemComponentV2(), PlayerState);

	if (!GetController<APlayerController>())
	{
		UE_LOG(LogPawnComponent, Error, TEXT("Cannot initialize component: PlayerController = NULL!"));
		return;
	}

	if (!Character->InputComponent)
	{
		UE_LOG(LogPawnComponent, Error, TEXT("Cannot initialize player input: InputComponent = NULL!"));
		return;
	}

	InitializePlayerInput(Character->InputComponent);

	if (const UPawnData* PawnData = Character->GetPawnData())
	{
		// TODO: setup camera component.
	}

	bPawnHasInitialized = true;
}

void UPlayerPawnComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag) const
{
	const AGameCharacter* GameCharacter = GetPawn<AGameCharacter>();
	check(GameCharacter);

	UAbilitySystemComponentV2* AbilitySystemComponent = GameCharacter->GetAbilitySystemComponentV2();
	check(AbilitySystemComponent);

	AbilitySystemComponent->AbilityInputTagPressed(InputTag);
}

void UPlayerPawnComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag) const
{
	const AGameCharacter* GameCharacter = GetPawn<AGameCharacter>();
	check(GameCharacter);

	UAbilitySystemComponentV2* AbilitySystemComponent = GameCharacter->GetAbilitySystemComponentV2();
	check(AbilitySystemComponent);

	AbilitySystemComponent->AbilityInputTagReleased(InputTag);
}

void UPlayerPawnComponent::UpdateCachedData()
{
	if (ControlledPawn = GetPawn<APawn>(); ControlledPawn)
	{
		Controller = ControlledPawn->GetController();
	}
}

void UPlayerPawnComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	if (!Controller || !ControlledPawn)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
	FVector MovementDirection;

	if (Value.X != 0.f)
	{
		MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
		ControlledPawn->AddMovementInput(MovementDirection, Value.X);
	}

	if (Value.Y != 0.f)
	{
		MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		ControlledPawn->AddMovementInput(MovementDirection, Value.Y);
	}
}

void UPlayerPawnComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	if (!ControlledPawn)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.f)
	{
		ControlledPawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.f)
	{
		ControlledPawn->AddControllerPitchInput(Value.Y);
	}
}

void UPlayerPawnComponent::Input_LookGamepad(const FInputActionValue& InputActionValue)
{
	if (!ControlledPawn)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.f)
	{
		ControlledPawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.f)
	{
		ControlledPawn->AddControllerPitchInput(Value.Y);
	}
}
