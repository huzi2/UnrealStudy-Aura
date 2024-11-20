// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraFireBlast.generated.h"

/**
 * ȭ�� ���� ��ų
 */
UCLASS()
class AURA_API UAuraFireBlast : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
	
private:
	// UAuraGameplayAbility���� ���
	virtual FString GetDescription(int32 Level) const final;
	virtual FString GetNextLevelDescription(int32 Level) const final;

protected:
	// ȭ���� ����
	UPROPERTY(EditDefaultsOnly, Category = "FireBlast")
	int32 NumFireBolls = 12;
};
