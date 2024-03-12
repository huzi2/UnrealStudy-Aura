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
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface) return;

	const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	Rotation.Pitch = 0.f;

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
			// �߻�ü�� ����Ʈ ���� �ڵ鿡 ����
			Projectile->DamageEffectSpecHandle = SourceAbilitySystemComponent->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), SourceAbilitySystemComponent->MakeEffectContext());

			// �߻�ü�� ��Ÿ ��Ʈ����Ʈ Damage�� �������� ����
			const UAuraGameplayTags GameplayTags = UAuraGameplayTags::Get();
			// �����Ƽ ������ ���� Ŀ�����̺� ����� �������� ���´�.
			const float ScaledDamage = Damage.GetValueAtLevel(static_cast<float>(GetAbilityLevel()));
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(Projectile->DamageEffectSpecHandle, GameplayTags.Damage, ScaledDamage);
		}
	}
	
	// �߻�ü ���� ����
	Projectile->FinishSpawning(SpawnTransform);
}
