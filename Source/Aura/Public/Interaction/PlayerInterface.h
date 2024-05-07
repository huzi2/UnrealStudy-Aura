// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 플레이어 캐릭터가 사용할 인터페이스
 */
class AURA_API IPlayerInterface
{
	GENERATED_BODY()

protected:
	// 경험치 관련
	UFUNCTION(BlueprintNativeEvent)
	int32 FindLevelForXP(int32 InXP) const;
	UFUNCTION(BlueprintNativeEvent)
	int32 GetXP() const;
	UFUNCTION(BlueprintNativeEvent)
	void AddToXP(int32 InXP);

	// 레벨 관련
	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();
	UFUNCTION(BlueprintNativeEvent)
	void AddToPlayerLevel(int32 InPlayerLevel);

	// 능력치 관련
	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePointReward(int32 Level) const;
	UFUNCTION(BlueprintNativeEvent)
	void AddToAttributePoints(int32 InAttributePoints);

	// 스킬포인트 관련
	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPointReward(int32 Level) const;
	UFUNCTION(BlueprintNativeEvent)
	void AddToSpellPoints(int32 InSpellPoints);
};
