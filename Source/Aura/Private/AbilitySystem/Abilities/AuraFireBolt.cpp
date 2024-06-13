// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/AuraFireBolt.h"
#include "AuraGameplayTags.h"

FString UAuraFireBolt::GetDescription(int32 Level) const
{
	// 파이어볼 스킬에 대한 설명

	// 레벨에 따라 변경되는 데미지(불속성만 얻어옴)
	const int32 Damage = static_cast<int32>(GetDamageByDamageType(static_cast<float>(Level), UAuraGameplayTags::Get().Damage_Fire));
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
			Level, ManaCost, Cooldown, Damage);
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
			Level, ManaCost, Cooldown, FMath::Min(Level, NumProjectiles), Damage);
	}
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level) const
{
	const int32 Damage = static_cast<int32>(GetDamageByDamageType(static_cast<float>(Level), UAuraGameplayTags::Get().Damage_Fire));
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
		Level, ManaCost, Cooldown, FMath::Min(Level, NumProjectiles), Damage);
}
