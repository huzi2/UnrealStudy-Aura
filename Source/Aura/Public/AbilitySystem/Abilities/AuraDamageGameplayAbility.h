// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Interaction/CombatInterface.h"
#include "AuraAbilityTypes.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
	
public:
	// 데미지 이펙트에서 사용할 구조체 생성
	FDamageEffectParams MakeDamageEffectParamsFromClassDefault(AActor* TargetActor = nullptr) const;

private:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const;

protected:
	// 데미지를 가하는 게임플레이 이펙트 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	// 스킬의 데미지 타입 태그
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag DamageTypeTag;
	// 스킬의 데미지 커브 테이블 값
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FScalableFloat Damage;
	
	// 디버프 확률
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffChance = 20.f;
	// 디버프 데미지
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffDamage = 5.f;
	// 디버프 지속시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float DebuffDuration = 5.f;
	// 디버프 주기
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffFrequency = 1.f;

	// 죽었을 때 충격 정도
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DeathImpulseMagnitude = 18000.f;
};
