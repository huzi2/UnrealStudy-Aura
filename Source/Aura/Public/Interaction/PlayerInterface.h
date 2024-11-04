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
 * �÷��̾� ĳ���Ͱ� ����� �������̽�
 */
class AURA_API IPlayerInterface
{
	GENERATED_BODY()

protected:
	// ����ġ ����
	UFUNCTION(BlueprintNativeEvent)
	int32 FindLevelForXP(int32 InXP) const;
	UFUNCTION(BlueprintNativeEvent)
	int32 GetXP() const;
	UFUNCTION(BlueprintNativeEvent)
	void AddToXP(int32 InXP);

	// ���� ����
	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();
	UFUNCTION(BlueprintNativeEvent)
	void AddToPlayerLevel(int32 InPlayerLevel);

	// �ɷ�ġ ����Ʈ ����
	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePoints() const;
	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePointReward(int32 Level) const;
	UFUNCTION(BlueprintNativeEvent)
	void AddToAttributePoints(int32 InAttributePoints);

	// ��ų ����Ʈ ����
	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPoints() const;
	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPointReward(int32 Level) const;
	UFUNCTION(BlueprintNativeEvent)
	void AddToSpellPoints(int32 InSpellPoints);

	// ������ ����
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ShowMagicCircle(UMaterialInstance* DecalMaterial = nullptr);
	// ������ ����
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HideMagicCircle();
};
