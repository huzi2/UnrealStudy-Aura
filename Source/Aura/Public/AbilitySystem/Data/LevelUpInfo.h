// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"

/**
 * ���� ���� �ʿ��� ������
 */
USTRUCT(BlueprintType)
struct FAuraLevelUpInfo
{
	GENERATED_BODY()

public:
	// �������� �ʿ��� ����ġ��
	UPROPERTY(EditDefaultsOnly)
	int32 LevelUpRequirement = 0;

	// �������ϸ� ���� �� �ִ� �ɷ�ġ ����Ʈ
	UPROPERTY(EditDefaultsOnly)
	int32 AttributePointAward = 1;

	// �������ϸ� ���� �� �ִ� ��ų ����Ʈ
	UPROPERTY(EditDefaultsOnly)
	int32 SpellPointAward = 1;
};

/**
 * ���� ������ ������ �ִ� ������ ���� Ŭ����
 */
UCLASS()
class AURA_API ULevelUpInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	FORCEINLINE const TArray<FAuraLevelUpInfo>& GetLevelUpInformation() const { return LevelUpInformation; }

	// ����ġ�� �´� ������ ����
	int32 FindLevelForXP(int32 XP) const;
	// �ִ� ����
	int32 GetMaxLevel() const;
	// �ش� ������ �´� ����
	const FAuraLevelUpInfo& GetLevelUpInfo(int32 Level) const;

private:
	UPROPERTY(EditDefaultsOnly)
	TArray<FAuraLevelUpInfo> LevelUpInformation;
};
