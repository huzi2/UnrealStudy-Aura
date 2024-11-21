// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraFireBlast.generated.h"

class AAuraFireBall;

/**
 * 화염 폭발 스킬
 */
UCLASS()
class AURA_API UAuraFireBlast : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
	
private:
	// UAuraGameplayAbility에서 상속
	virtual FString GetDescription(int32 Level) const final;
	virtual FString GetNextLevelDescription(int32 Level) const final;

protected:
	// 화염구들을 발사
	UFUNCTION(BlueprintCallable)
	TArray<AAuraFireBall*> SpawnFireBalls();

protected:
	// 화염구 개수
	UPROPERTY(EditDefaultsOnly, Category = "FireBlast")
	int32 NumFireBolls = 12;

private:
	// 화염 폭발에서 사용할 발사체 클래스
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAuraFireBall> FireBallClass;
};
