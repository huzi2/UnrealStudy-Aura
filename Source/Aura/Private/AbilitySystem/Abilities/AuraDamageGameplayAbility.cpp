// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	if (!DamageEffectClass) return;
	if (!TargetActor) return;

	// 데미지 이펙트 스펙 핸들 생성
	const FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);

	// 데미지 타입별로 적용
	for (const TTuple<FGameplayTag, FScalableFloat>& Pair : DamageTypes)
	{
		// 어빌리티 레벨에 따라 커브테이블에 적용된 데미지를 얻어온다.
		const float ScaledDamage = Pair.Value.GetValueAtLevel(static_cast<float>(GetAbilityLevel()));		

		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, Pair.Key, ScaledDamage);
	}

	// 타겟 액터에게 데미지 이펙트 적용
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}
