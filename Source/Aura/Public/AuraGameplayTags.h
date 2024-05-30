// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// 게임플레이 태그를 변수로 관리하는 싱글톤 클래스
class UAuraGameplayTags
{
public:
	static const UAuraGameplayTags& Get() { return GameplayTags; }

	static void InitializeNativeGameplayTags();

public:
	// 능력치 관련 태그
	// 주요 능력
	FGameplayTag Attribute_Primary_Strength;
	FGameplayTag Attribute_Primary_Intelligence;
	FGameplayTag Attribute_Primary_Resilience;
	FGameplayTag Attribute_Primary_Vigor;

	// 보조 능력
	FGameplayTag Attribute_Secondary_Armor;
	FGameplayTag Attribute_Secondary_ArmorPenetration;
	FGameplayTag Attribute_Secondary_BlockChance;
	FGameplayTag Attribute_Secondary_CriticalHitChance;
	FGameplayTag Attribute_Secondary_CriticalHitDamage;
	FGameplayTag Attribute_Secondary_CriticalHitResistance;
	FGameplayTag Attribute_Secondary_HealthRegeneration;
	FGameplayTag Attribute_Secondary_ManaRegeneration;
	FGameplayTag Attribute_Secondary_MaxHealth;
	FGameplayTag Attribute_Secondary_MaxMana;

	// 속성 저항력
	FGameplayTag Attribute_Resistance_Fire;
	FGameplayTag Attribute_Resistance_Lightning;
	FGameplayTag Attribute_Resistance_Arcane;
	FGameplayTag Attribute_Resistance_Physical;

	// 메타 어트리뷰트
	FGameplayTag Attribute_Meta_IncomingXP;

	// 인풋 태그
	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;
	FGameplayTag InputTag_Passive_1;
	FGameplayTag InputTag_Passive_2;

	// 데미지 종류
	FGameplayTag Damage;
	FGameplayTag Damage_Fire;
	FGameplayTag Damage_Lightning;
	FGameplayTag Damage_Arcane;
	FGameplayTag Damage_Physical;
	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;

	// 이펙트
	FGameplayTag Effects_HitReact;

	// 어빌리티 관련 태그
	// 어빌리티
	FGameplayTag Abilities_Attack;
	FGameplayTag Abilities_Summon;

	// 플레이어가 사용할 속성 어빌리티
	FGameplayTag Abilities_Fire_FireBolt;
	FGameplayTag Abilities_Lightning_Electrocute;

	// 기타 어빌리티
	FGameplayTag Abilities_HitReact;

	// 어빌리티 상태
	FGameplayTag Abilities_Status_Locked;
	FGameplayTag Abilities_Status_Eligible;
	FGameplayTag Abilities_Status_Unlocked;
	FGameplayTag Abilities_Status_Equipped;

	// 어빌리티 종류
	FGameplayTag Abilities_Type_Offensive;
	FGameplayTag Abilities_Type_Passive;
	FGameplayTag Abilities_Type_None;

	// 어빌리티 쿨타임
	FGameplayTag Cooldown_Fire_FireBolt;

	// 공격 소켓
	FGameplayTag CombatSocket_Weapon;
	FGameplayTag CombatSocket_LeftHand;
	FGameplayTag CombatSocket_RightHand;
	FGameplayTag CombatSocket_Tail;

	// 몬스터 공격 종류
	FGameplayTag Montage_Attack1;
	FGameplayTag Montage_Attack2;
	FGameplayTag Montage_Attack3;
	FGameplayTag Montage_Attack4;

private:
	static UAuraGameplayTags GameplayTags;
};
