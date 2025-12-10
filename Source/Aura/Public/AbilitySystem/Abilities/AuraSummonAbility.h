// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraSummonAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraSummonAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
	
private:
	UAuraSummonAbility();

private:
	UFUNCTION(BlueprintPure)
	TArray<FVector> GetSpawnLocations() const;

	UFUNCTION(BlueprintPure)
	TSubclassOf<APawn> GetRandomMinionClass() const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	int32 NumMinions;

	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	float MinSpawnDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	float MaxSpawnDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	float SpawnSpread;

	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	TArray<TSubclassOf<APawn>> MinionClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	bool bDrawDebug;
};
