// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/AuraFireBlast.h"

FString UAuraFireBlast::GetDescription(int32 Level) const
{
	// 화염 폭발 스킬에 대한 설명

	// 레벨에 따라 변경되는 데미지(불속성만 얻어옴)
	const int32 ScaledDamage = static_cast<int32>(Damage.GetValueAtLevel(static_cast<float>(Level)));
	// 마나 코스트
	const float ManaCost = FMath::Abs(GetManaCost(static_cast<float>(Level)));
	// 쿨타임
	const float Cooldown = GetCooldown(static_cast<float>(Level));

	return FString::Printf(TEXT(
		// 스킬명
		"<Title>FIRE BLAST</>\n\n"
		// 스킬 능력치
		"<Small>Level: </><Level>%d</>\n"
		"<Small>ManaCost: </><Manacost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		// 설명
		"<Default>Launches %d</>"
		"<Default>fire balls in all directions, each coming back and</>"
		"<Default>exploding upon return, causing </>"
		// 데미지
		"<Damage>%d</><Default> radial fire damage with"
		" a chance to burn</>\n\n"),
		Level, ManaCost, Cooldown, NumFireBolls, ScaledDamage);
}

FString UAuraFireBlast::GetNextLevelDescription(int32 Level) const
{
	const int32 ScaledDamage = static_cast<int32>(Damage.GetValueAtLevel(static_cast<float>(Level)));
	const float ManaCost = FMath::Abs(GetManaCost(static_cast<float>(Level)));
	const float Cooldown = GetCooldown(static_cast<float>(Level));

	return FString::Printf(TEXT(
		// 스킬명
		"<Title>NEXT LEVEL: </>\n\n"
		// 스킬 능력치
		"<Small>Level: </><Level>%d</>\n"
		"<Small>ManaCost: </><Manacost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		// 설명
		"<Default>Launches %d</>"
		"<Default>fire balls in all directions, each coming back and</>"
		"<Default>exploding upon return, causing </>"
		// 데미지
		"<Damage>%d</><Default> radial fire damage with"
		" a chance to burn</>\n\n"),
		Level, ManaCost, Cooldown, NumFireBolls, ScaledDamage);
}
