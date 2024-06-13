// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/AuraAttributeSet.h"

FString UAuraGameplayAbility::GetDescription(int32 Level) const
{
    // RichText ������ ���ؼ� �����Ƽ�� ������ ����
    return FString::Printf(TEXT("<Default>%s, </><Level>%d</>"), TEXT("Default Ability Nmae - LoremIpusm LoremIpusm LoremIpusm LoremIpusm LoremIpusm LoremIpusm LoremIpusm"), Level);
}

FString UAuraGameplayAbility::GetNextLevelDescription(int32 Level) const
{
    return FString::Printf(TEXT("<Default>Next Level: </><Level>%d</> \n<Default>Caues much more damage </>"), Level);
}

FString UAuraGameplayAbility::GetLockedDescription(int32 Level)
{
    return FString::Printf(TEXT("<Default>Spell Locked Until Level: %d</>"), Level);
}

float UAuraGameplayAbility::GetManaCost(float InLevel) const
{
    float ManaCost = 0.f;

    // �����Ƽ�� ���õ� �ڽ�Ʈ ����Ʈ���� ���� ���� ��Ʈ����Ʈ�� ���´�
    if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())
    {
        for (const FGameplayModifierInfo& Mod : CostEffect->Modifiers)
        {
            // ���� ��Ʈ����Ʈ���� ���� �ű״�Ʃ�尪�� ���´�
            if (Mod.Attribute == UAuraAttributeSet::GetManaAttribute())
            {
                Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost);
                break;
            }
        }
    }
    return ManaCost;
}

float UAuraGameplayAbility::GetCooldown(float InLevel) const
{
    float Cooldown = 0.f;

    // �����Ƽ�� ���õ� ��Ÿ�� ����Ʈ���� DurationMagnitude�� ���� ������ �װ� ��Ÿ��
    if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
    {
        CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
    }
    return Cooldown;
}
