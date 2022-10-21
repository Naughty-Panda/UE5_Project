// Bartender Ender
// Naughty Panda @ 2022

#include "HealthComponent.h"
#include "GameplayEffectExtension.h"
#include "BartenderEnder/CoreGameplayTags.h"
#include "BartenderEnder/AbilitySystem/AbilitySystemComponentV2.h"
#include "BartenderEnder/AbilitySystem/Attributes/HealthSet.h"

DEFINE_LOG_CATEGORY_STATIC(LogHealthComponent, Log, All);

UHealthComponent::UHealthComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

bool UHealthComponent::TryInitializeWithAbilitySystem(UAbilitySystemComponentV2* InAbilitySystemComponent)
{
	const AActor* Owner = GetOwner();

	if (!Owner)
	{
		UE_LOG(LogHealthComponent, Error, TEXT("Cannot initialize, component has no owner."));
		return false;
	}

	if (AbilitySystemComponent)
	{
		UE_LOG(LogHealthComponent, Error, TEXT("Health component for %s have already been initialized!"), *GetNameSafe(Owner));
		return false;
	}

	AbilitySystemComponent = InAbilitySystemComponent;
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogHealthComponent, Error, TEXT("Cannot initialize Health component for %s with NULL Ability System!"), *GetNameSafe(Owner));
		return false;
	}

	HealthSet = AbilitySystemComponent->GetSet<UHealthSet>();
	if (!HealthSet)
	{
		UE_LOG(LogHealthComponent, Error, TEXT("Cannot initialize component for %s with NULL HealthSet!"), *GetNameSafe(Owner));
		AbilitySystemComponent = nullptr;
		return false;
	}

	// Register to listen for attribute changes.
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHealthSet::GetHealthAttribute()).AddUObject(this, &ThisClass::HealthChangedHelper);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHealthSet::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::MaxHealthChangedHelper);
	HealthSet->OnOutOfHealth.AddUObject(this, &ThisClass::OutOfHealthHelper);

	// Reset attributes to default values. TODO: use DataTable?
	AbilitySystemComponent->SetNumericAttributeBase(UHealthSet::GetHealthAttribute(), HealthSet->GetHealth());
	AbilitySystemComponent->SetNumericAttributeBase(UHealthSet::GetMaxHealthAttribute(), HealthSet->GetMaxHealth());

	ClearGameplayTags();

	// Broadcast initialized values.
	OnHealthChanged.Broadcast(this, nullptr, HealthSet->GetHealth(), HealthSet->GetHealth());
	OnMaxHealthChanged.Broadcast(this, nullptr, HealthSet->GetMaxHealth(), HealthSet->GetMaxHealth());
	return true;
}

void UHealthComponent::UninitializeFromAbilitySystem()
{
	ClearGameplayTags();

	if (HealthSet)
	{
		HealthSet->OnOutOfHealth.RemoveAll(this);
	}

	HealthSet = nullptr;
	AbilitySystemComponent = nullptr;
}

float UHealthComponent::GetHealth() const
{
	return HealthSet ? HealthSet->GetHealth() : 0.f;
}

float UHealthComponent::GetMaxHealth() const
{
	return HealthSet ? HealthSet->GetMaxHealth() : 0.f;
}

float UHealthComponent::GetHealthNormalized() const
{
	if (!HealthSet)
	{
		return 0.f;
	}

	const float Health = HealthSet->GetHealth();
	const float MaxHealth = HealthSet->GetMaxHealth();
	return MaxHealth > 0.f ? Health / MaxHealth : 0.f;
}

void UHealthComponent::StartDeath()
{
	if (IsDeadOrDying())
	{
		UE_LOG(LogHealthComponent, Warning, TEXT("Cannot start death: health component's owner is already dead or dying."));
		return;
	}

	ApplyDeathState_Internal(EDeathState::Dying);
}

void UHealthComponent::FinishDeath()
{
	if (DeathState != EDeathState::Dying)
	{
		UE_LOG(LogHealthComponent, Warning, TEXT("Cannot finish death: health component's owner is not dying!"));
		return;
	}

	ApplyDeathState_Internal(EDeathState::Dead);
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UHealthComponent::OnUnregister()
{
	UninitializeFromAbilitySystem();
	Super::OnUnregister();
}

void UHealthComponent::ClearGameplayTags() const
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	const FCoreGameplayTags& GameplayTags = FCoreGameplayTags::Get();
	AbilitySystemComponent->SetLooseGameplayTagCount(GameplayTags.Status_Death_Dying, 0);
	AbilitySystemComponent->SetLooseGameplayTagCount(GameplayTags.Status_Death_Dead, 0);
}

static AActor* GetInstigatorFromAttrChangeData(const FOnAttributeChangeData& ChangeData)
{
	if (ChangeData.GEModData)
	{
		const FGameplayEffectContextHandle& EffectContext = ChangeData.GEModData->EffectSpec.GetEffectContext();
		return EffectContext.GetOriginalInstigator();
	}

	return nullptr;
}

void UHealthComponent::HealthChangedHelper(const FOnAttributeChangeData& ChangeData)
{
	OnHealthChanged.Broadcast(this, GetInstigatorFromAttrChangeData(ChangeData), ChangeData.OldValue, ChangeData.NewValue);
}

void UHealthComponent::MaxHealthChangedHelper(const FOnAttributeChangeData& ChangeData)
{
	OnMaxHealthChanged.Broadcast(this, GetInstigatorFromAttrChangeData(ChangeData), ChangeData.OldValue, ChangeData.NewValue);
}

void UHealthComponent::OutOfHealthHelper(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageValue)
{
	// TODO: Skip DamageCauser?

	if (!AbilitySystemComponent)
	{
		UE_LOG(LogHealthComponent, Error, TEXT("Cannot handle OutOfHealth event: AbilitySystemComponent = NULL!"));
		return;
	}

	FGameplayEventData EventData;
	EventData.EventTag = FCoreGameplayTags::Get().GameplayEvent_Death;
	EventData.Instigator = DamageInstigator;
	EventData.Target = AbilitySystemComponent->GetAvatarActor();
	EventData.OptionalObject = DamageEffectSpec.Def;
	EventData.ContextHandle = DamageEffectSpec.GetEffectContext();
	EventData.InstigatorTags = *DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
	EventData.TargetTags = *DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();
	EventData.EventMagnitude = DamageValue;

	AbilitySystemComponent->HandleGameplayEvent(EventData.EventTag, &EventData);
}

void UHealthComponent::ApplyDeathState_Internal(EDeathState NewDeathState)
{
	const FDeathEvent* DeathEvent = nullptr;
	const FGameplayTag* NewDeathStatusTag = nullptr;

	switch (NewDeathState)
	{
	case EDeathState::Dying:

		DeathEvent = &OnDying;
		DeathState = EDeathState::Dying;
		NewDeathStatusTag = &FCoreGameplayTags::Get().Status_Death_Dying;
		break;

	case EDeathState::Dead:

		DeathEvent = &OnDeath;
		DeathState = EDeathState::Dead;
		NewDeathStatusTag = &FCoreGameplayTags::Get().Status_Death_Dead;
		break;

	default: break;
	}

	if (!AbilitySystemComponent)
	{
		UE_LOG(LogHealthComponent, Error, TEXT("Cannot set required gameplay tag: AbilitySystemComponent = NULL."));
		return;
	}

	check(NewDeathStatusTag);
	AbilitySystemComponent->SetLooseGameplayTagCount(*NewDeathStatusTag, 1);

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		UE_LOG(LogHealthComponent, Error, TEXT("Health component has no owner!"));
	}

	check(DeathEvent);
	DeathEvent->Broadcast(Owner);
}
