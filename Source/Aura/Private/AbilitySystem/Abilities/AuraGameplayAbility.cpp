// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

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
