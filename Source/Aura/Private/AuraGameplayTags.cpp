// Fill out your copyright notice in the Description page of Project Settings.

#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"

UAuraGameplayTags UAuraGameplayTags::GameplayTags;

void UAuraGameplayTags::InitializeNativeGameplayTags()
{
	// 필요한 태그들 태그 매니저에 추가

	// 주요 속성 태그들
	GameplayTags.Attribute_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Attributes.Primary.Strength"), TEXT("Increases physical damage"));
	GameplayTags.Attribute_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Attributes.Primary.Intelligence"), TEXT("Increases magical damage"));
	GameplayTags.Attribute_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Attributes.Primary.Resilience"), TEXT("Increases Armor and Armor Penetration"));
	GameplayTags.Attribute_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Attributes.Primary.Vigor"), TEXT("Increases Health"));

	// 2차 속성 태그들
	GameplayTags.Attribute_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Attributes.Secondary.Armor"), TEXT("Reduces damage taken, improves Block Chance"));
	GameplayTags.Attribute_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Attributes.Secondary.ArmorPenetration"), TEXT("Ignores Percentage of enemy Armor, increases Critical Hit Chance"));
	GameplayTags.Attribute_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Attributes.Secondary.BlockChance"), TEXT("Chance to cut incoming damage in half"));
	GameplayTags.Attribute_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Attributes.Secondary.CriticalHitChance"), TEXT("Chance to double damage plus critical hit bonus"));
	GameplayTags.Attribute_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Attributes.Secondary.CriticalHitDamage"), TEXT("Bonus damage added when a critical hit is scored"));
	GameplayTags.Attribute_Secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Attributes.Secondary.CriticalHitResistance"), TEXT("Reduces Critical Hit Chance of attacking enemies"));
	GameplayTags.Attribute_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Attributes.Secondary.HealthRegeneration"), TEXT("Amount of Health regenerated every 1 second"));
	GameplayTags.Attribute_Secondary_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Attributes.Secondary.ManaRegeneration"), TEXT("Amount of Mana regenerated every 1 second"));
	GameplayTags.Attribute_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Attributes.Secondary.MaxHealth"), TEXT("Maximum amount of Health obtainable"));
	GameplayTags.Attribute_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Attributes.Secondary.MaxMana"), TEXT("Maximum amount of Mana obtainable"));

	// 데미지 타입 저항 태그들
	GameplayTags.Attribute_Resistance_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Attributes.Resistance.Fire"), TEXT("Resistance to Fire Damage"));
	GameplayTags.Attribute_Resistance_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Attributes.Resistance.Lightning"), TEXT("Resistance to Lightning Damage"));
	GameplayTags.Attribute_Resistance_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Attributes.Resistance.Arcane"), TEXT("Resistance to Arcane Damage"));
	GameplayTags.Attribute_Resistance_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Attributes.Resistance.Physical"), TEXT("Resistance to Physical Damage"));

	// 입력 태그들
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("InputTag.LMB"), TEXT("Input Tag for Left Mouse Button"));
	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("InputTag.RMB"), TEXT("Input Tag for Left Mouse Button"));
	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("InputTag.1"), TEXT("Input Tag for 1 key"));
	GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("InputTag.2"), TEXT("Input Tag for 2 key"));
	GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("InputTag.3"), TEXT("Input Tag for 3 key"));
	GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("InputTag.4"), TEXT("Input Tag for 4 key"));

	// 데미지를 입히는 어빌리티 태그
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Damage"), TEXT("Damage"));
	GameplayTags.Damage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Damage.Fire"), TEXT("Fire Damage Type"));
	GameplayTags.Damage_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Damage.Lightning"), TEXT("Lightning Damage Type"));
	GameplayTags.Damage_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Damage.Arcane"), TEXT("Arcane Damage Type"));
	GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Damage.Physical"), TEXT("Fire Damage Type"));
	
	// 데미지 타입과 데미지 타입 저항을 맵에 연결
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Fire, GameplayTags.Attribute_Resistance_Fire);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Lightning, GameplayTags.Attribute_Resistance_Lightning);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Arcane, GameplayTags.Attribute_Resistance_Arcane);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Physical, GameplayTags.Attribute_Resistance_Physical);

	// 맞았을 때 타겟에게 부여해서 피격 모션 출력하게하는 태그
	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Effects.HitReact"), TEXT("Tag granted when Hit Reacting"));

	// 어빌리티들
	GameplayTags.Abilities_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Abilities.Attack"), TEXT("Attack Ability Tag"));

	// 몽타주 소켓
	GameplayTags.Montage_Attack_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Montage.Attack.Weapon"), TEXT("Attack by Weapon"));
	GameplayTags.Montage_Attack_LeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Montage.Attack.LeftHand"), TEXT("Attack by Left Hand"));
	GameplayTags.Montage_Attack_RightHand = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Montage.Attack.RightHand"), TEXT("Attack by Right Hand"));
}
