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

	// 입력 태그들
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("InputTag.LMB"), TEXT("Input Tag for Left Mouse Button"));
	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("InputTag.RMB"), TEXT("Input Tag for Left Mouse Button"));
	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("InputTag.1"), TEXT("Input Tag for 1 key"));
	GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("InputTag.2"), TEXT("Input Tag for 2 key"));
	GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("InputTag.3"), TEXT("Input Tag for 3 key"));
	GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("InputTag.4"), TEXT("Input Tag for 4 key"));
}
