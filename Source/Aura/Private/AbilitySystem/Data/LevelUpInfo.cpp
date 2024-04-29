// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(int32 XP) const
{
	int32 Level = 1;

	bool bSearching = true;
	while (bSearching)
	{
		if (LevelUpInformation.Num() - 1 <= Level) return Level;

		if (XP >= LevelUpInformation[Level].LevelUpRequirement)
		{
			++Level;
		}
		else
		{
			bSearching = false;
		}
	}
	return Level;
}

int32 ULevelUpInfo::GetMaxLevel() const
{
	return LevelUpInformation.Num();
}

const FAuraLevelUpInfo& ULevelUpInfo::GetLevelUpInfo(int32 Level) const
{
	return LevelUpInformation[Level];
}
