// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/AuraFireBlast.h"

FString UAuraFireBlast::GetDescription(int32 Level) const
{
	// ȭ�� ���� ��ų�� ���� ����

	// ������ ���� ����Ǵ� ������(�ҼӼ��� ����)
	const int32 ScaledDamage = static_cast<int32>(Damage.GetValueAtLevel(static_cast<float>(Level)));
	// ���� �ڽ�Ʈ
	const float ManaCost = FMath::Abs(GetManaCost(static_cast<float>(Level)));
	// ��Ÿ��
	const float Cooldown = GetCooldown(static_cast<float>(Level));

	return FString::Printf(TEXT(
		// ��ų��
		"<Title>FIRE BLAST</>\n\n"
		// ��ų �ɷ�ġ
		"<Small>Level: </><Level>%d</>\n"
		"<Small>ManaCost: </><Manacost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		// ����
		"<Default>Launches %d</>"
		"<Default>fire balls in all directions, each coming back and</>"
		"<Default>exploding upon return, causing </>"
		// ������
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
		// ��ų��
		"<Title>NEXT LEVEL: </>\n\n"
		// ��ų �ɷ�ġ
		"<Small>Level: </><Level>%d</>\n"
		"<Small>ManaCost: </><Manacost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		// ����
		"<Default>Launches %d</>"
		"<Default>fire balls in all directions, each coming back and</>"
		"<Default>exploding upon return, causing </>"
		// ������
		"<Damage>%d</><Default> radial fire damage with"
		" a chance to burn</>\n\n"),
		Level, ManaCost, Cooldown, NumFireBolls, ScaledDamage);
}
