// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

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
