// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AuraGameInstance.generated.h"

/**
 * ���� �ν��Ͻ� Ŭ����
 */
UCLASS()
class AURA_API UAuraGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	FORCEINLINE const FName& GetPlayerStartTag() const { return PlayerStartTag; }

	FORCEINLINE void SetPlayerStartTag(const FName& InPlayerStartTag) { PlayerStartTag = InPlayerStartTag; }
	FORCEINLINE void SetLoadSlotName(const FString& InLoadSlotName) { LoadSlotName = InLoadSlotName; }
	FORCEINLINE void SetLoadSlotIndex(int32 InLoadSlotIndex) { LoadSlotIndex = InLoadSlotIndex; }

private:
	// ������ ���Կ��� ���� ������
	UPROPERTY()
	FName PlayerStartTag = FName();
	UPROPERTY()
	FString LoadSlotName = FString();
	UPROPERTY()
	int32 LoadSlotIndex = 0;
};
