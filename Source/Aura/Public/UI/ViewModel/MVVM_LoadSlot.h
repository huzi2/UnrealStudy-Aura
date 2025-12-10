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
	const FString& GetSlotName() const { return SlotName; }
	int32 GetPlayerLevel() const { return PlayerLevel; }
	FORCEINLINE int32 GetSlotIndex() const { return SlotIndex; }
	FORCEINLINE const FName& GetPlayerStartTag() const { return PlayerStartTag; }
	FORCEINLINE const FString& GetMapAssetName() const { return MapAssetName; }

	void SetPlayerName(const FString& InPlayerName);
	void SetMapName(const FString& InMapName);
	void SetSlotName(const FString& InSlotName);
	void SetPlayerLevel(int32 InPlayerLevel);
	FORCEINLINE void SetSlotIndex(int32 InSlotIndex) { SlotIndex = InSlotIndex; }
	FORCEINLINE void SetSlotStatus(ESaveSlotStatus InSlotStatus) { SlotStatus = InSlotStatus; }
	FORCEINLINE void SetPlayerStartTag(const FName& InPlayerStartTag) { PlayerStartTag = InPlayerStartTag; }
	FORCEINLINE void SetMapAssetName(const FString& InMapAssetName) { MapAssetName = InMapAssetName; }

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 PlayerLevel;
	// 뷰모델 생성하기 위한 필드 알림용 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter)
	FString SlotName;

private:
	// 슬롯을 세이브할 때 사용할 변수들
	UPROPERTY()
	int32 SlotIndex;

	// 현재 로드 슬롯의 상태
	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SlotStatus;

	// 슬롯에서 마지막으로 사용한 플레이어 스타트 태그
	UPROPERTY()
	FName PlayerStartTag;

	// 마지막으로 있었던 맵
	UPROPERTY()
	FString MapAssetName;
};
