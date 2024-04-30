// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Data/CharacterClassInfo.h"

const FCharacterClassDefaultInfo& UCharacterClassInfo::GetClassDefaultInfo(ECharacterClass CharacterClass) const
{
	return CharacterClassInformation.FindChecked(CharacterClass);
}
