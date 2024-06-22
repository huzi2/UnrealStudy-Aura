// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/AuraFireBolt.h"
#include "AuraGameplayTags.h"

FString UAuraFireBolt::GetDescription(int32 Level) const
{
	// ���̾ ��ų�� ���� ����

	// ������ ���� ����Ǵ� ������(�ҼӼ��� ����)
	const int32 Damage = static_cast<int32>(GetDamageByDamageType(static_cast<float>(Level), UAuraGameplayTags::Get().Damage_Fire));
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
			Level, ManaCost, Cooldown, Damage);
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
		Level, ManaCost, Cooldown, FMath::Min(Level, NumProjectiles), Damage);
}