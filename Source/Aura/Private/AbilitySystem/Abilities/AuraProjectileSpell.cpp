// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	if (!GetAvatarActorFromActorInfo()) return;

	// �߻�ü ������ ���������� ����
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;
	if (!GetWorld()) return;

	// ���� ���ð� ������ �����Ƽ�� ���� Ŭ������ ���ӵ��� �ʱ� ���ؼ� �������̽��� ����Ѵ�.
	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo());
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());

	// �����÷��� ����Ʈ�� ������Ѿ� �ϹǷ� SpawnActorDeferred()�� ������ �̷��.
	AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform, GetOwningActorFromActorInfo(), Cast<APawn>(GetOwningActorFromActorInfo()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!Projectile) return;

	// ������ �����Ƽ �ý��� ������Ʈ�� ����ؼ� �����Ƽ�� ������ �����÷��� ����Ʈ ���� �ڵ��� ����
	if (const UAbilitySystemComponent* SourceAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo()))
	{
		if (DamageEffectClass)
		{
			// ����Ʈ�� ������ ä��� ���� �����÷��� ���ؽ�Ʈ �ڵ��� ����
			FGameplayEffectContextHandle EffectContextHandle = SourceAbilitySystemComponent->MakeEffectContext();
			// �Ʒ� ����� �����÷��� ���ؽ�Ʈ �ڵ�� ������ �͵��� ��Ƴ��� ��. �� ���ӿ����� �ǹ̴� ����.
			// ����Ʈ�� �߻���Ų �����Ƽ ���
			EffectContextHandle.SetAbility(this);
			// ����Ʈ�� �߻���Ų ��ü(�߻�ü) ���
			EffectContextHandle.AddSourceObject(Projectile);
			// ����Ʈ�� ���õ� ���͵� ������ �߻�ü ���
			TArray<TWeakObjectPtr<AActor>> Actors;
			Actors.Add(Projectile);
			EffectContextHandle.AddActors(Actors);
			// �浹ó�� ���� ����� ���� �� �浹ó�� ����� ����(����� �������)
			FHitResult HitResult;
			HitResult.Location = ProjectileTargetLocation;
			EffectContextHandle.AddHitResult(HitResult);

			// �߻�ü�� ����Ʈ ���� �ڵ鿡 ����
			Projectile->DamageEffectSpecHandle = SourceAbilitySystemComponent->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

			// �߻�ü�� ��Ÿ ��Ʈ����Ʈ Damage�� �������� ����
			const UAuraGameplayTags GameplayTags = UAuraGameplayTags::Get();

			// ������ Ÿ�Ժ��� ����
			for (const auto& Pair : DamageTypes)
			{
				// �����Ƽ ������ ���� Ŀ�����̺� ����� �������� ���´�.
				const float ScaledDamage = Pair.Value.GetValueAtLevel(static_cast<float>(GetAbilityLevel()));

				UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(Projectile->DamageEffectSpecHandle, Pair.Key, ScaledDamage);
			}			
		}
	}
	
	// �߻�ü ���� ����
	Projectile->FinishSpawning(SpawnTransform);
}
