// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "MVVM_LoadSlot.generated.h"

// 위젯 스위처 전환할 때 사용할 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndex, int32, WidgetSwitcherIndex);
// Taken의 슬롯 선택 버튼을 활성/비활성화할 때 사용할 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnableSelectSlotButton, bool, bEnable);

/**
 * 로드 메뉴에서 각각의 로드 슬롯과 연결된 뷰모델 클래스
 */
UCLASS()
class AURA_API UMVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
public:
	const FString& GetPlayerName() const { return PlayerName; }
	const FString& GetMapName() const { return MapName; }
	FORCEINLINE const FString& GetSlotName() const { return SlotName; }
	FORCEINLINE int32 GetSlotIndex() const { return SlotIndex; }

	void SetPlayerName(const FString& InPlayerName);
	void SetMapName(const FString& InMapName);
	FORCEINLINE void SetSlotName(const FString& InSlotName) { SlotName = InSlotName; }	
	FORCEINLINE void SetSlotIndex(int32 InSlotIndex) { SlotIndex = InSlotIndex; }
	FORCEINLINE void SetSlotStatus(ESaveSlotStatus InSlotStatus) { SlotStatus = InSlotStatus; }

	void InitializeSlot();

public:
	// 위젯 스위처 전환할 때 사용할 델리게이트
	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndex SetWidgetSwitcherIndexDelegate;
	// Taken의 슬롯 선택 버튼을 활성/비활성화할 때 사용할 델리게이트
	UPROPERTY(BlueprintAssignable)
	FEnableSelectSlotButton EnableSelectSlotButtonDelegate;

protected:
	// UI와 바인딩할 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter)
	FString PlayerName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter)
	FString MapName;

private:
	// 슬롯을 세이브할 때 사용할 변수들
	UPROPERTY()
	FString SlotName;
	UPROPERTY()
	int32 SlotIndex;

	// 현재 로드 슬롯의 상태
	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SlotStatus;
};
