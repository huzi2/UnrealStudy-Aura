// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/AuraFireBolt.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

FString UAuraFireBolt::GetDescription(int32 Level) const
{
	// ȭ���� ��ų�� ���� ����

	// ������ ���� ����Ǵ� ������(�ҼӼ��� ����)
	const int32 ScaledDamage = static_cast<int32>(Damage.GetValueAtLevel(static_cast<float>(Level)));
	// ���� �ڽ�Ʈ
	const float ManaCost = FMath::Abs(GetManaCost(static_cast<float>(Level)));
	// ��Ÿ��
	const float Cooldown = GetCooldown(static_cast<float>(Level));

	// 1������ �ϳ��� �߻�
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			// ��ų��
			"<Title>FIRE BOLT</>\n\n"
			// ��ų �ɷ�ġ
			"<Small>Level: </><Level>%d</>\n"
			"<Small>ManaCost: </><Manacost>%.1f</>\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			// ����
			"<Default>Launches a bolt of fire, "
			"exploding on impact and dealing: </>"
			// ������
			"<Damage>%d</><Default> fire damage with"
			" a chance to burn</>\n\n"), 
			Level, ManaCost, Cooldown, ScaledDamage);
	}
	// �� �Ŀ��� ������ ���� �߻�ü�� ����
	else
	{
		return FString::Printf(TEXT(
			// ��ų��
			"<Title>FIRE BOLT</>\n\n"
			// ��ų �ɷ�ġ
			"<Small>Level: </><Level>%d</>\n"
			"<Small>ManaCost: </><Manacost>%.1f</>\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			// ����(�߻簹��)
			"<Default>Launches %d bolts of fire, "
			"exploding on impact and dealing: </>"
			// ������
			"<Damage>%d</><Default> fire damage with"
			" a chance to burn</>\n\n"),
			Level, ManaCost, Cooldown, FMath::Min(Level, MaxNumProjectiles), ScaledDamage);
	}
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level) const
{
	const int32 ScaledDamage = static_cast<int32>(Damage.GetValueAtLevel(static_cast<float>(Level)));
	const float ManaCost = FMath::Abs(GetManaCost(static_cast<float>(Level)));
	const float Cooldown = GetCooldown(static_cast<float>(Level));

	return FString::Printf(TEXT(
		"<Title>NEXT LEVEL: </>\n\n"
		// ��ų �ɷ�ġ
		"<Small>Level: </><Level>%d</>\n"
		"<Small>ManaCost: </><Manacost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		// ����(�߻簹��)
		"<Default>Launches %d bolts of fire, "
		"exploding on impact and dealing: </>"
		// ������
		"<Damage>%d</><Default> fire damage with"
		" a chance to burn</>\n\n"),
		Level, ManaCost, Cooldown, FMath::Min(Level, MaxNumProjectiles), ScaledDamage);
}

void UAuraFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride, AActor* HomingTarget)
{
	if (!GetAvatarActorFromActorInfo()) return;

	// �߻�ü ������ ���������� ����
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;
	if (!GetWorld()) return;

	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();

	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}

	const FVector Forward = Rotation.Vector();

	// �߻�ü ������ ���� ~ MaxNumProjectiles
	 NumProjectiles = FMath::Min(GetAbilityLevel(), MaxNumProjectiles);

	const TArray<FRotator> Rotations = UAuraAbilitySystemLibrary::EvenlySpreadRotators(Forward, FVector::UpVector, ProjectileSpread, NumProjectiles);
	for (const FRotator& Direction : Rotations)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Direction.Quaternion());

		// �����÷��� ����Ʈ�� ������Ѿ� �ϹǷ� SpawnActorDeferred()�� ������ �̷��.
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform, GetOwningActorFromActorInfo(), Cast<APawn>(GetOwningActorFromActorInfo()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (!Projectile) return;

		Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefault();

		if (Projectile->GetProjectileMovement())
		{
			if (HomingTarget && HomingTarget->Implements<UCombatInterface>())
			{
				// Ÿ�� ����
				Projectile->GetProjectileMovement()->HomingTargetComponent = HomingTarget->GetRootComponent();
			}
			else
			{
				// Ÿ���� ������ ����ִ� �ӽ� ������Ʈ�� ����
				Projectile->SetHomingTargetSceneComponent(NewObject<USceneComponent>(USceneComponent::StaticClass()));
				Projectile->GetHomingTargetSceneComponent()->SetWorldLocation(ProjectileTargetLocation);
				Projectile->GetProjectileMovement()->HomingTargetComponent = Projectile->GetHomingTargetSceneComponent();
			}

			Projectile->GetProjectileMovement()->HomingAccelerationMagnitude = FMath::FRandRange(HomingAccelerationMin, HomingAccelerationMax);
			Projectile->GetProjectileMovement()->bIsHomingProjectile = bLaunchHomingProjectiles;
		}

		// �߻�ü ���� ����
		Projectile->FinishSpawning(SpawnTransform);
	}
}
