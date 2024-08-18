// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/AuraFireBolt.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"

FString UAuraFireBolt::GetDescription(int32 Level) const
{
	// 파이어볼 스킬에 대한 설명

	// 레벨에 따라 변경되는 데미지(불속성만 얻어옴)
	const int32 ScaledDamage = static_cast<int32>(Damage.GetValueAtLevel(static_cast<float>(Level)));
	// 마나 코스트
	const float ManaCost = FMath::Abs(GetManaCost(static_cast<float>(Level)));
	// 쿨타임
	const float Cooldown = GetCooldown(static_cast<float>(Level));

	// 1레벨은 하나만 발사
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			// 스킬명
			"<Title>FIRE BOLT</>\n\n"
			// 스킬 능력치
			"<Small>Level: </><Level>%d</>\n"
			"<Small>ManaCost: </><Manacost>%.1f</>\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			// 설명
			"<Default>Launches a bolt of fire, "
			"exploding on impact and dealing: </>"
			// 데미지
			"<Damage>%d</><Default> fire damage with"
			" a chance to burn</>\n\n"), 
			Level, ManaCost, Cooldown, ScaledDamage);
	}
	// 그 후에는 레벨에 따라 발사체가 증가
	else
	{
		return FString::Printf(TEXT(
			// 스킬명
			"<Title>FIRE BOLT</>\n\n"
			// 스킬 능력치
			"<Small>Level: </><Level>%d</>\n"
			"<Small>ManaCost: </><Manacost>%.1f</>\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			// 설명(발사갯수)
			"<Default>Launches %d bolts of fire, "
			"exploding on impact and dealing: </>"
			// 데미지
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
		// 스킬 능력치
		"<Small>Level: </><Level>%d</>\n"
		"<Small>ManaCost: </><Manacost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		// 설명(발사갯수)
		"<Default>Launches %d bolts of fire, "
		"exploding on impact and dealing: </>"
		// 데미지
		"<Damage>%d</><Default> fire damage with"
		" a chance to burn</>\n\n"),
		Level, ManaCost, Cooldown, FMath::Min(Level, MaxNumProjectiles), ScaledDamage);
}

void UAuraFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride, AActor* HomingTarget)
{
	if (!GetAvatarActorFromActorInfo()) return;

	// 발사체 생성은 서버에서만 진행
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;
	if (!GetWorld()) return;

	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();

	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}

	const FVector Forward = Rotation.Vector();

	// 발사체 개수는 레벨 ~ MaxNumProjectiles
	// NumProjectiles = FMath::Min(GetAbilityLevel(), MaxNumProjectiles);
	NumProjectiles = 5;

	const TArray<FRotator> Rotations = UAuraAbilitySystemLibrary::EvenlySpreadRotators(Forward, FVector::UpVector, ProjectileSpread, NumProjectiles);
	for (const FRotator& Direction : Rotations)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Direction.Quaternion());

		// 게임플레이 이펙트를 적용시켜야 하므로 SpawnActorDeferred()로 생성을 미룬다.
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform, GetOwningActorFromActorInfo(), Cast<APawn>(GetOwningActorFromActorInfo()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (!Projectile) return;

		Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefault();

		// 발사체 이제 생성
		Projectile->FinishSpawning(SpawnTransform);
	}
}
