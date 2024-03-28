// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	if (!DamageEffectClass) return;
	if (!TargetActor) return;

	// ������ ����Ʈ ���� �ڵ� ����
	const FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);

	// ������ Ÿ�Ժ��� ����
	for (const TTuple<FGameplayTag, FScalableFloat>& Pair : DamageTypes)
	{
		// �����Ƽ ������ ���� Ŀ�����̺� ����� �������� ���´�.
		const float ScaledDamage = Pair.Value.GetValueAtLevel(static_cast<float>(GetAbilityLevel()));		

		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, Pair.Key, ScaledDamage);
	}

	// Ÿ�� ���Ϳ��� ������ ����Ʈ ����
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}
