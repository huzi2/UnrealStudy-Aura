// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "AuraFireBolt.generated.h"

/**
 * 화염구 스킬 클래스
 */
UCLASS()
class AURA_API UAuraFireBolt : public UAuraProjectileSpell
{
	GENERATED_BODY()
	
private:
	// UAuraGameplayAbility에서 상속
	virtual FString GetDescription(int32 Level) const final;
	virtual FString GetNextLevelDescription(int32 Level) const final;

private:
	// 발사체 최대 갯수. 발사체 갯수는 레벨을 따라가고 최대 5개까지 증가
	UPROPERTY(EditDefaultsOnly)
	int32 NumProjectiles = 5;
};
