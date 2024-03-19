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
	// Ŀ���� �����÷��� ����Ʈ ���ؽ�Ʈ �ڵ��� ��������Ƿ� �װ� ����ϱ� ���� �� �Լ��� ������
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const final;
};
