// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefault(
	AActor* TargetActor,
	FVector InRadialDamageOrigin,
	bool bOverriedKnockbackDirection,
	FVector InKnockbackDirectionOverried,
	bool bOverriedDeathImpulse,
	FVector InDeathImpulseOverried,
	bool bOverriedPitch,
	float InPitchOverried) const
{
	// ������ ����Ʈ���� ����� ����ü ���� ����
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageTypeTag = DamageTypeTag;
	Params.DebuffChance = DebuffChance;
	Params.DebuffDamage = DebuffDamage;
	Params.DebuffDuration = DebuffDuration;
	Params.DebuffFrequency = DebuffFrequency;
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
	Params.KnockbackChance = KnockbackChance;
	Params.KnockbackForceMagnitude = KnockbackForceMagnitude;

	// �߻�ü���� �ϴ� �浹 ���� ����� �߻�ü �ƴ� ������ �ϱ� ���ؼ� ���
	if (IsValid(TargetActor))
	{
		FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		if (bOverriedPitch)
		{
			Rotation.Pitch = InPitchOverried;
		}
		const FVector ToTarget = Rotation.Vector();

		if (!bOverriedKnockbackDirection)
		{
			Params.KnockbackForce = ToTarget * KnockbackForceMagnitude;
		}
		if (!bOverriedDeathImpulse)
		{
			Params.DeathImpulse = ToTarget * DeathImpulseMagnitude;
		}
	}

	// �Ű������� ����� ��� ���
	if (bOverriedKnockbackDirection)
	{
		InKnockbackDirectionOverried.Normalize();
		Params.KnockbackForce = InKnockbackDirectionOverried * KnockbackForceMagnitude;

		if (bOverriedPitch)
		{
			FRotator KnockbackRotation = InKnockbackDirectionOverried.Rotation();
			KnockbackRotation.Pitch = InPitchOverried;
			Params.KnockbackForce = KnockbackRotation.Vector() * KnockbackForceMagnitude;
		}
	}
	if (bOverriedDeathImpulse)
	{
		InDeathImpulseOverried.Normalize();
		Params.DeathImpulse = InDeathImpulseOverried * DeathImpulseMagnitude;

		if (bOverriedPitch)
		{
			FRotator DeathImpulseRotation = InDeathImpulseOverried.Rotation();
			DeathImpulseRotation.Pitch = InPitchOverried;
			Params.DeathImpulse = DeathImpulseRotation.Vector() * DeathImpulseMagnitude;
		}
	}

	// ����� ������ ���� ���� ����
	if (bIsRadialDamage)
	{
		Params.bIsRadialDamage = bIsRadialDamage;
		Params.RadialDamageInnerRadius = RadialDamageInnerRadius;
		Params.RadialDamageOuterRadius = RadialDamageOuterRadius;
		Params.RadialDamageOrigin = InRadialDamageOrigin;
	}
	return Params;
}

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	if (!DamageEffectClass) return;
	if (!TargetActor) return;

	// ������ ����Ʈ ���� �ڵ� ����
	const FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);

	// �����Ƽ ������ ���� Ŀ�����̺� ����� �������� ���´�.
	const float ScaledDamage = Damage.GetValueAtLevel(static_cast<float>(GetAbilityLevel()));
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, DamageTypeTag, ScaledDamage);

	// Ÿ�� ���Ϳ��� ������ ����Ʈ ����
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

FTaggedMontage UAuraDamageGameplayAbility::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const
{
	if (TaggedMontages.Num() > 0)
	{
		const int32 Selection = FMath::RandRange(0, TaggedMontages.Num() - 1);
		return TaggedMontages[Selection];
	}
	return FTaggedMontage();
}

float UAuraDamageGameplayAbility::GetDamageAtLevel() const
{
	return Damage.GetValueAtLevel(static_cast<float>(GetAbilityLevel()));
}
