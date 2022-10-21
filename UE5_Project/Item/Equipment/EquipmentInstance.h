// Bartender Ender
// Naughty Panda @ 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EquipmentInstance.generated.h"

class UItemSubsystem;
struct FEquipmentActor;

/**
 * Equipment Instance owned by a pawn.
 */
UCLASS(BlueprintType, Blueprintable)
class UEquipmentInstance : public UObject
{
	GENERATED_BODY()

public:
	UEquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual UWorld* GetWorld() const override final;
	UItemSubsystem* GetItemSubsystem() const;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	UObject* GetInstigator() const { return Instigator; }

	void SetInstigator(UObject* InInstigator) { Instigator = InInstigator; }

	UFUNCTION(BlueprintPure, Category = "Equipment")
	APawn* GetPawn() const { return Cast<APawn>(GetOuter()); }

	UFUNCTION(BlueprintPure, Category = "Equipment", Meta = (DeterminesOutputType = PawnType))
	APawn* GetPawnTyped(TSubclassOf<APawn> PawnType) const;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	TArray<AActor*> GetSpawnedActors() const { return SpawnedActors; }

	virtual void SpawnEquipmentActors(const TArray<FEquipmentActor>& ActorsToSpawn);
	virtual void DestroyEquipmentActors();
	virtual void SetEquipmentActorsHiddenInGame(bool bNewHidden);

	virtual void OnEquipped() { Script_OnEquipped(); }
	virtual void OnUnequipped() { Script_OnUnequipped(); }

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment", Meta = (DisplayName = "OnEquipped"))
	void Script_OnEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment", Meta = (DisplayName = "OnUnequipped"))
	void Script_OnUnequipped();

private:
	UPROPERTY()
	TObjectPtr<UObject> Instigator = nullptr;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> SpawnedActors;
};
