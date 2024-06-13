// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "AuraFireBolt.generated.h"

/**
 * ȭ���� ��ų Ŭ����
 */
UCLASS()
class AURA_API UAuraFireBolt : public UAuraProjectileSpell
{
	GENERATED_BODY()
	
private:
	// UAuraGameplayAbility���� ���
	virtual FString GetDescription(int32 Level) const final;
	virtual FString GetNextLevelDescription(int32 Level) const final;

private:
	// �߻�ü �ִ� ����. �߻�ü ������ ������ ���󰡰� �ִ� 5������ ����
	UPROPERTY(EditDefaultsOnly)
	int32 NumProjectiles = 5;
};
