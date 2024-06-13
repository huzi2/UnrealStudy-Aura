// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Interaction/CombatInterface.h"
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

	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const;

protected:
	float GetDamageByDamageType(float InLevel, const FGameplayTag& DamageType) const;

protected:
	// �������� ���ϴ� �����÷��� ����Ʈ Ŭ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	// ������ Ÿ��(�±׷� ����)�� ������ Ŀ�� ���̺� ��
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;
};
