// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AuraGameInstance.generated.h"

/**
 * 게임 인스턴스 클래스
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
	// 생성한 슬롯에서 얻어온 정보들
	UPROPERTY()
	FName PlayerStartTag = FName();
	UPROPERTY()
	FString LoadSlotName = FString();
	UPROPERTY()
	int32 LoadSlotIndex = 0;
};
