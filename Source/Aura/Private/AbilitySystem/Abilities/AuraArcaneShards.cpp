// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraArcaneShards.h"

FString UAuraArcaneShards::GetDescription(int32 Level) const
{
	// 비전 파편 스킬에 대한 설명

	// 레벨에 따라 변경되는 데미지
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
			"<Title>ARCANE SHARDS</>\n\n"
			// 스킬 능력치
			"<Small>Level: </><Level>%d</>\n"
			"<Small>ManaCost: </><Manacost>%.1f</>\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			// 설명
			"<Default>Summon a shard of arcane energy, "
			"causing radial damage of </>"
			// 데미지
			"<Damage>%d</><Default>"
			" at the shard origin</>\n\n"),
			Level, ManaCost, Cooldown, ScaledDamage);
	}
	// 그 후에는 레벨에 따라 발사체가 증가
	else
	{
		return FString::Printf(TEXT(
			// 스킬명
			"<Title>ARCANE SHARDS</>\n\n"
			// 스킬 능력치
			"<Small>Level: </><Level>%d</>\n"
			"<Small>ManaCost: </><Manacost>%.1f</>\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			// 설명(발사갯수)
			"<Default>Summon %d shards of arcane energy, "
			"causing radial damage of </>"
			// 데미지
			"<Damage>%d</><Default>"
			" at the shard origin</>\n\n"),
			Level, ManaCost, Cooldown, FMath::Min(Level, MaxNumShards), ScaledDamage);
	}
}

FString UAuraArcaneShards::GetNextLevelDescription(int32 Level) const
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
		"<Default>Summon %d shards of arcane energy, "
		"causing radial damage of </>"
		// 데미지
		"<Damage>%d</><Default>"
		" at the shard origin</>\n\n"),
		Level, ManaCost, Cooldown, FMath::Min(Level, MaxNumShards), ScaledDamage);
}
