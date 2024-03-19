// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "AuraAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
	
private:
	// 커스텀 게임플레이 이펙트 컨텍스트 핸들을 만들었으므로 그걸 사용하기 위해 이 함수를 재정의
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const final;
};
