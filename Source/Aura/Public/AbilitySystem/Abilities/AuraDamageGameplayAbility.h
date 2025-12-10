// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Interaction/CombatInterface.h"
#include "AuraAbilityTypes.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 데미지를 가진 어빌리티
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
	
public:
	// 데미지 이펙트에서 사용할 구조체 생성
	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromClassDefault(
		AActor* TargetActor = nullptr, 
		FVector InRadialDamageOrigin = FVector::ZeroVector, 
		bool bOverriedKnockbackDirection = false, 
		FVector InKnockbackDirectionOverried = FVector::ZeroVector,
		bool bOverriedDeathImpulse = false, 
		FVector InDeathImpulseOverried = FVector::ZeroVector, 
		bool bOverriedPitch = false, 
		float InPitchOverried = 0.f) const;

private:
	// 타겟에게 데미지 적용
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

	// 태그와 연결된 몽타주 랜덤 선택
	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const;

	// 스킬의 설정된 데미지 값을 얻어옴
	UFUNCTION(BlueprintPure)
	float GetDamageAtLevel() const;

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
	float DeathImpulseMagnitude = 1000.f;

	// 넉백 확률
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float KnockbackChance = 0.f;
	// 넉백 충격 정도
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float KnockbackForceMagnitude = 1000.f;

	// 방사형 데미지 관련
	// 방사형 데미지인가?
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	bool bIsRadialDamage = false;
	// 방사형 데미지 내각 크기
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float RadialDamageInnerRadius = 0.f;
	// 방사형 데미지 외곽 크기
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float RadialDamageOuterRadius = 0.f;
};
