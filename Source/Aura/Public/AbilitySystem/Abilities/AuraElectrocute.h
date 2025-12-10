// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraBeamSpell.h"
#include "AuraElectrocute.generated.h"

/**
 * 감전 스킬 클래스
 */
UCLASS()
class AURA_API UAuraElectrocute : public UAuraBeamSpell
{
	GENERATED_BODY()
	
private:
	// UAuraGameplayAbility에서 상속
	virtual FString GetDescription(int32 Level) const final;
	virtual FString GetNextLevelDescription(int32 Level) const final;
};
