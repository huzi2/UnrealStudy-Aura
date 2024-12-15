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
	const FString& GetPlayerName() const { return PlayerName; }
	const FString& GetMapName() const { return MapName; }
	const FString& GetSlotName() const { return SlotName; }
	int32 GetPlayerLevel() const { return PlayerLevel; }
	FORCEINLINE int32 GetSlotIndex() const { return SlotIndex; }

	void SetPlayerName(const FString& InPlayerName);
	void SetMapName(const FString& InMapName);
	void SetSlotName(const FString& InSlotName);
	void SetPlayerLevel(int32 InPlayerLevel);
	FORCEINLINE void SetSlotIndex(int32 InSlotIndex) { SlotIndex = InSlotIndex; }
	FORCEINLINE void SetSlotStatus(ESaveSlotStatus InSlotStatus) { SlotStatus = InSlotStatus; }

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter)
	FString MapName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 PlayerLevel;
	// ��� �����ϱ� ���� �ʵ� �˸��� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter)
	FString SlotName;

private:
	// ������ ���̺��� �� ����� ������
	UPROPERTY()
	int32 SlotIndex;

	// ���� �ε� ������ ����
	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SlotStatus;
};
