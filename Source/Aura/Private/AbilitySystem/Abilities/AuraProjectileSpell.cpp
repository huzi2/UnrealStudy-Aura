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

FString UAuraProjectileSpell::GetDescription(int32 Level) const
{
	// ���̾ ��ų�� ���� ����

	// ������ ���� ����Ǵ� ������(�ҼӼ��� ����)
	const int32 Damage = static_cast<int32>(DamageTypes[UAuraGameplayTags::Get().Damage_Fire].GetValueAtLevel(Level));

	// 1������ �ϳ��� �߻�
	if (Level == 1)
	{
		return FString::Printf(TEXT("<Title>FIRE BOLT</>\n\n<Default>Launched a bolt of fire, exploding on impact and dealing: </><Damage>%d</><Default> fire damage with a chance to burn</>\n\n<Small>Level: </><Level>%d</>"), Damage, Level);
	}
	// �� �Ŀ��� ������ ���� �߻�ü�� ����
	else
	{
		return FString::Printf(TEXT("<Title>FIRE BOLT</>\n\n<Default>Launched %d bolts of fire, exploding on impact and dealing: </><Damage>%d</><Default> fire damage with a chance to burn</>\n\n<Small>Level: </><Level>%d</>"), FMath::Min(Level, NumProjectiles), Damage, Level);
	}
}

FString UAuraProjectileSpell::GetNextLevelDescription(int32 Level) const
{
	const int32 Damage = static_cast<int32>(DamageTypes[UAuraGameplayTags::Get().Damage_Fire].GetValueAtLevel(Level));
	return FString::Printf(TEXT("<Title>NEXT LEVEL: </>\n\n<Default>Launched %d bolts of fire, exploding on impact and dealing: </><Damage>%d</><Default> fire damage with a chance to burn</>\n\n<Small>Level: </><Level>%d</>"), FMath::Min(Level, NumProjectiles), Damage, Level);
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride)
{
	if (!GetAvatarActorFromActorInfo()) return;

	// �߻�ü ������ ���������� ����
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;
	if (!GetWorld()) return;

	// ���� ���ð� ������ �����Ƽ�� ���� Ŭ������ ���ӵ��� �ʱ� ���ؼ� �������̽��� ����Ѵ�.
	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();

	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}

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

			// ������ Ÿ�Ժ��� ����
			for (const TTuple<FGameplayTag, FScalableFloat>& Pair : DamageTypes)
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
