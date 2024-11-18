// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraArcaneShards.h"

FString UAuraArcaneShards::GetDescription(int32 Level) const
{
	// ���� ���� ��ų�� ���� ����

	// ������ ���� ����Ǵ� ������
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
			"<Title>ARCANE SHARDS</>\n\n"
			// ��ų �ɷ�ġ
			"<Small>Level: </><Level>%d</>\n"
			"<Small>ManaCost: </><Manacost>%.1f</>\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			// ����
			"<Default>Summon a shard of arcane energy, "
			"causing radial damage of </>"
			// ������
			"<Damage>%d</><Default>"
			" at the shard origin</>\n\n"),
			Level, ManaCost, Cooldown, ScaledDamage);
	}
	// �� �Ŀ��� ������ ���� �߻�ü�� ����
	else
	{
		return FString::Printf(TEXT(
			// ��ų��
			"<Title>ARCANE SHARDS</>\n\n"
			// ��ų �ɷ�ġ
			"<Small>Level: </><Level>%d</>\n"
			"<Small>ManaCost: </><Manacost>%.1f</>\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			// ����(�߻簹��)
			"<Default>Summon %d shards of arcane energy, "
			"causing radial damage of </>"
			// ������
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
		// ��ų �ɷ�ġ
		"<Small>Level: </><Level>%d</>\n"
		"<Small>ManaCost: </><Manacost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		// ����(�߻簹��)
		"<Default>Summon %d shards of arcane energy, "
		"causing radial damage of </>"
		// ������
		"<Damage>%d</><Default>"
		" at the shard origin</>\n\n"),
		Level, ManaCost, Cooldown, FMath::Min(Level, MaxNumShards), ScaledDamage);
}
