// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/AuraAttributeSet.h"

FString UAuraGameplayAbility::GetDescription(int32 Level) const
{
    // RichText 서식을 통해서 어빌리티의 설명을 생성
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

    // 어빌리티에 세팅된 코스트 이펙트에서 마나 관련 어트리뷰트를 얻어온다
    if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())
    {
        for (const FGameplayModifierInfo& Mod : CostEffect->Modifiers)
        {
            // 마나 어트리뷰트에서 마나 매그니튜드값을 얻어온다
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

    // 어빌리티에 세팅된 쿨타임 이펙트에서 DurationMagnitude의 값을 얻으면 그게 쿨타임
    if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
    {
        CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
    }
    return Cooldown;
}
