// Fill out your copyright notice in the Description page of Project Settings.

#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"

constexpr int32 DAMAGE_TYPE_MAX = 4;

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

	// 메타 속성 태그들
	GameplayTags.Attribute_Meta_IncomingXP = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Attributes.Meta.IncomingXP"), TEXT("Incoming XP Meta Attribute"));

	// 입력 태그들
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("InputTag.LMB"), TEXT("Input Tag for Left Mouse Button"));
	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("InputTag.RMB"), TEXT("Input Tag for Left Mouse Button"));
	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("InputTag.1"), TEXT("Input Tag for 1 key"));
	GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("InputTag.2"), TEXT("Input Tag for 2 key"));
	GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("InputTag.3"), TEXT("Input Tag for 3 key"));
	GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("InputTag.4"), TEXT("Input Tag for 4 key"));
	GameplayTags.InputTag_Passive_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("InputTag.Passive.1"), TEXT("Input Tag for Passive 1"));
	GameplayTags.InputTag_Passive_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("InputTag.Passive.2"), TEXT("Input Tag for Passive 2"));

	// 데미지를 입히는 어빌리티 태그
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Damage"), TEXT("Damage"));
	GameplayTags.Damage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Damage.Fire"), TEXT("Fire Damage Type"));
	GameplayTags.Damage_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Damage.Lightning"), TEXT("Lightning Damage Type"));
	GameplayTags.Damage_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Damage.Arcane"), TEXT("Arcane Damage Type"));
	GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Damage.Physical"), TEXT("Fire Damage Type"));

	// 데미지 타입과 데미지 타입 저항을 맵에 연결
	GameplayTags.DamageTypesToResistances.Reserve(DAMAGE_TYPE_MAX);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Fire, GameplayTags.Attribute_Resistance_Fire);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Lightning, GameplayTags.Attribute_Resistance_Lightning);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Arcane, GameplayTags.Attribute_Resistance_Arcane);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Physical, GameplayTags.Attribute_Resistance_Physical);

	// 디버프 태그들
	GameplayTags.Debuff_Burn = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Debuff.Burn"), TEXT("Debuff for Fire Damage"));
	GameplayTags.Debuff_Stun = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Debuff.Stun"), TEXT("Debuff for Lightning Damage"));
	GameplayTags.Debuff_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Debuff.Arcane"), TEXT("Debuff for Arcane Damage"));
	GameplayTags.Debuff_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Debuff.Physical"), TEXT("Debuff for Physical Damage"));

	// 데미지 타입과 디버프 태그을 맵에 연결
	GameplayTags.DamageTypesToDebuffs.Reserve(DAMAGE_TYPE_MAX);
	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Fire, GameplayTags.Debuff_Burn);
	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Lightning, GameplayTags.Debuff_Stun);
	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Arcane, GameplayTags.Debuff_Arcane);
	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Physical, GameplayTags.Debuff_Physical);

	// 디버프 변수들
	GameplayTags.Debuff_Chance = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Debuff.Chance"), TEXT("Debuff Chance"));
	GameplayTags.Debuff_Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Debuff.Damage"), TEXT("Debuff Damage"));
	GameplayTags.Debuff_Duration = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Debuff.Duration"), TEXT("Debuff Duration"));
	GameplayTags.Debuff_Frequency = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Debuff.Frequency"), TEXT("Debuff Frequency"));

	// 맞았을 때 타겟에게 부여해서 피격 모션 출력하게하는 태그
	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Effects.HitReact"), TEXT("Tag granted when Hit Reacting"));

	// 어빌리티들
	GameplayTags.Abilities_None = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Abilities.None"), TEXT("No Ability - like the nullptr for Ability Tags"));

	GameplayTags.Abilities_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Abilities.Attack"), TEXT("Attack Ability Tag"));
	GameplayTags.Abilities_Summon = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Abilities.Summon"), TEXT("Summon Ability Tag"));
	GameplayTags.Abilities_Fire_FireBolt = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Abilities.Fire.FireBolt"), TEXT("FireBolt Ability Tag"));
	GameplayTags.Abilities_Lightning_Electrocute = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Abilities.Lightning.Electrocute"), TEXT("Electrocute Ability Tag"));
	GameplayTags.Abilities_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Abilities.HitReact"), TEXT("HitReact Ability Tag"));

	// 어빌리티 상태
	GameplayTags.Abilities_Status_Locked = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Abilities.Status.Locked"), TEXT("Locked Status"));
	GameplayTags.Abilities_Status_Eligible = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Abilities.Status.Eligible"), TEXT("Eligible Status"));
	GameplayTags.Abilities_Status_Unlocked = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Abilities.Status.Unlocked"), TEXT("Unlocked Status"));
	GameplayTags.Abilities_Status_Equipped = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Abilities.Status.Equipped"), TEXT("Equipped Status"));

	// 어빌리티 종류
	GameplayTags.Abilities_Type_Offensive = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Abilities.Type.Offensive"), TEXT("Type Offensive"));
	GameplayTags.Abilities_Type_Passive = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Abilities.Type.Passive"), TEXT("Type Passive"));
	GameplayTags.Abilities_Type_None = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Abilities.Type.None"), TEXT("Type None"));

	// 어빌리티의 쿨타임
	GameplayTags.Cooldown_Fire_FireBolt = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Cooldown.Fire.FireBolt"), TEXT("FireBolt Cooldown Tag"));

	// 공격 몽타주 소켓
	GameplayTags.CombatSocket_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("CombatSocket.Weapon"), TEXT("Attack by Weapon"));
	GameplayTags.CombatSocket_LeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("CombatSocket.LeftHand"), TEXT("Attack by Left Hand"));
	GameplayTags.CombatSocket_RightHand = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("CombatSocket.RightHand"), TEXT("Attack by Right Hand"));
	GameplayTags.CombatSocket_Tail = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("CombatSocket.Tail"), TEXT("Attack by Tail"));

	// 공격 몽타주 태그
	GameplayTags.Montage_Attack1 = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Montage.Attack1"), TEXT("Attack 1"));
	GameplayTags.Montage_Attack2 = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Montage.Attack2"), TEXT("Attack 2"));
	GameplayTags.Montage_Attack3 = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Montage.Attack3"), TEXT("Attack 3"));
	GameplayTags.Montage_Attack4 = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Montage.Attack4"), TEXT("Attack 4"));

	// 해당 태그가 있으면 해당 입력을 막음
	GameplayTags.Player_Block_InputPressed = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Player.Block.InputPressed"), TEXT("Block Input Pressed callback for input"));
	GameplayTags.Player_Block_InputHeld = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Player.Block.InputHeld"), TEXT("Block Input Held callback for input"));
	GameplayTags.Player_Block_InputReleased = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Player.Block.InputReleased"), TEXT("Block Input Released callback for input"));
	GameplayTags.Player_Block_CursorTrace = UGameplayTagsManager::Get().AddNativeGameplayTag(TEXT("Player.Block.CursorTrace"), TEXT("Block tracing under the cursor"));
}
