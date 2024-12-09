// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "MVVM_LoadSlot.generated.h"

// ���� ����ó ��ȯ�� �� ����� ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndex, int32, WidgetSwitcherIndex);
// Taken�� ���� ���� ��ư�� Ȱ��/��Ȱ��ȭ�� �� ����� ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnableSelectSlotButton, bool, bEnable);

/**
 * �ε� �޴����� ������ �ε� ���԰� ����� ��� Ŭ����
 */
UCLASS()
class AURA_API UMVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
public:
	FORCEINLINE const FString& GetSlotName() const { return SlotName; }
	FORCEINLINE void SetSlotName(const FString& InSlotName) { SlotName = InSlotName; }

	FORCEINLINE void SetSlotStatus(ESaveSlotStatus InSlotStatus) { SlotStatus = InSlotStatus; }

	const FString& GetPlayerName() const { return PlayerName; }
	void SetPlayerName(const FString& InPlayerName);

	void InitializeSlot();

public:
	// ���� ����ó ��ȯ�� �� ����� ��������Ʈ
	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndex SetWidgetSwitcherIndexDelegate;
	// Taken�� ���� ���� ��ư�� Ȱ��/��Ȱ��ȭ�� �� ����� ��������Ʈ
	UPROPERTY(BlueprintAssignable)
	FEnableSelectSlotButton EnableSelectSlotButtonDelegate;

protected:
	// UI�� ���ε��� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter)
	FString PlayerName;

private:
	// ������ ���̺��� �� ����� ������
	UPROPERTY()
	FString SlotName;
	UPROPERTY()
	int32 SlotIndex;

	// ���� �ε� ������ ����
	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SlotStatus;
};
