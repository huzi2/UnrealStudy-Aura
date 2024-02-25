// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_MaxMana.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMMC_MaxMana : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
private:
	UMMC_MaxMana();

private:
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const final;

private:
	FGameplayEffectAttributeCaptureDefinition IntelligenceDef;
};
