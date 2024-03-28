// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
	
private:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

protected:
	// �������� ���ϴ� �����÷��� ����Ʈ Ŭ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	// ������ Ÿ��(�±׷� ����)�� ������ Ŀ�� ���̺� ��
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;
};
