// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "BartenderEnder/AbilitySystem/AbilitySystemComponentV2.h"
#include "BartenderEnder/Character/PawnData.h"
#include "GameFramework/PlayerState.h"
#include "GamePlayerState.generated.h"

class UPawnData;

/**
 * Base Player State class for Bartender Ender.
 */
UCLASS(Blueprintable, Config = Game)
class AGamePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGamePlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual UAbilitySystemComponentV2* GetAbilitySystemComponentV2() const { return AbilitySystemComponent; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return GetAbilitySystemComponentV2(); }

	const UPawnData* GetPawnData() const { return PawnData; }
	void SetPawnData(const UPawnData* InPawnData);

	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void ClientInitialize(AController* C) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Reset() override;

protected:
	UPROPERTY()
	const UPawnData* PawnData = nullptr;

private:
	UPROPERTY(VisibleAnywhere, Category = "PlayerState")
	UAbilitySystemComponentV2* AbilitySystemComponent = nullptr;
};
