// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/AuraFireBolt.h"
#include "AuraGameplayTags.h"
#include "Kismet/KismetSystemLibrary.h"

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
	// 멀티 샷
	if (NumProjectiles > 1)
	{
		const FVector LeftOfSpread = Forward.RotateAngleAxis(-ProjectileSpread / 2.f, FVector::UpVector);
		const FVector RightOfSpread = Forward.RotateAngleAxis(ProjectileSpread / 2.f, FVector::UpVector);
		const float DeltaSpread = ProjectileSpread / (NumProjectiles - 1);

		for (int32 i = 0; i < NumProjectiles; ++i)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);

			UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), SocketLocation, SocketLocation + Direction * 75.f, 1.f, FLinearColor::Red, 120.f, 1.f);
		}

		UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), SocketLocation, SocketLocation + LeftOfSpread * 100.f, 1.f, FLinearColor::Gray, 120.f, 1.f);
		UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), SocketLocation, SocketLocation + RightOfSpread * 100.f, 1.f, FLinearColor::Gray, 120.f, 1.f);
	}
	// 싱글 샷
	else
	{
		
	}

	UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), SocketLocation, SocketLocation + Forward * 100.f, 1.f, FLinearColor::White, 120.f, 1.f);
}
