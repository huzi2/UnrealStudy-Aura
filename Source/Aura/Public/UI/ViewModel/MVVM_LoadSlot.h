// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadSlot.generated.h"

// 위젯 스위처 전환할 때 사용할 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndex, int32, WidgetSwitcherIndex);

/**
 * 로드 메뉴에서 각각의 로드 슬롯과 연결된 뷰모델 클래스
 */
UCLASS()
class AURA_API UMVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
public:
	FORCEINLINE const FString& GetSlotName() const { return SlotName; }
	FORCEINLINE const FString& GetPlayerName() const { return PlayerName; }

	FORCEINLINE void SetSlotName(const FString& InSlotName) { SlotName = InSlotName; }
	FORCEINLINE void SetPlayerName(const FString& InPlayerName) { PlayerName = InPlayerName; }

	void InitializeSlot();

public:
	// 위젯 스위처 전환할 때 사용할 델리게이트
	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndex SetWidgetSwitcherIndexDelegate;

private:
	// 슬롯을 세이브할 때 사용할 변수들
	UPROPERTY()
	FString SlotName;
	UPROPERTY()
	FString PlayerName;
	UPROPERTY()
	int32 SlotIndex;
};
