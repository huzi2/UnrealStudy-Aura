// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"

/**
 * 레벨 업에 필요한 정보들
 */
USTRUCT(BlueprintType)
struct FAuraLevelUpInfo
{
	GENERATED_BODY()

public:
	// 레벨업에 필요한 경험치량
	UPROPERTY(EditDefaultsOnly)
	int32 LevelUpRequirement = 0;

	// 레벨업하면 얻을 수 있는 능력치 포인트
	UPROPERTY(EditDefaultsOnly)
	int32 AttributePointAward = 1;

	// 레벨업하면 얻을 수 있는 스킬 포인트
	UPROPERTY(EditDefaultsOnly)
	int32 SpellPointAward = 1;
};

/**
 * 레벨 정보를 가지고 있는 데이터 에셋 클래스
 */
UCLASS()
class AURA_API ULevelUpInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	FORCEINLINE const TArray<FAuraLevelUpInfo>& GetLevelUpInformation() const { return LevelUpInformation; }

	// 경험치에 맞는 레벨을 얻어옴
	int32 FindLevelForXP(int32 XP) const;
	// 최대 레벨
	int32 GetMaxLevel() const;
	// 해당 레벨에 맞는 정보
	const FAuraLevelUpInfo& GetLevelUpInfo(int32 Level) const;

private:
	UPROPERTY(EditDefaultsOnly)
	TArray<FAuraLevelUpInfo> LevelUpInformation;
};
