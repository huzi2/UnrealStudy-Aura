// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadSlot.generated.h"

// ���� ����ó ��ȯ�� �� ����� ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndex, int32, WidgetSwitcherIndex);

/**
 * �ε� �޴����� ������ �ε� ���԰� ����� ��� Ŭ����
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
	// ���� ����ó ��ȯ�� �� ����� ��������Ʈ
	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndex SetWidgetSwitcherIndexDelegate;

private:
	// ������ ���̺��� �� ����� ������
	UPROPERTY()
	FString SlotName;
	UPROPERTY()
	FString PlayerName;
	UPROPERTY()
	int32 SlotIndex;
};
