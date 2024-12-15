// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadScreen.generated.h"

class UMVVM_LoadSlot;

// ������ ���õ� �� �˷��� ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSlotSelected);

/**
 * �ε� �޴��� ����� ��� Ŭ����
 */
UCLASS()
class AURA_API UMVVM_LoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	int32 GetNumLoadSlot() const { return NumLoadSlot; }
	void SetNumLoadSlot(int32 InNumLoadSlot);

	// �ε� ���� ��𵨵� ����
	void InitializeLoadSlot();

	// �ε� ���� ��������
	UFUNCTION(BlueprintPure)
	UMVVM_LoadSlot* GetLoadSlotViewModelByIndex(int32 Index) const;

	// �� ���� ��ư�� ������ ��
	UFUNCTION(BlueprintCallable)
	void NewSlotButtonPressed(int32 Slot, const FString& EnteredName);

	// �� ���� ��ư�� ������ ��
	UFUNCTION(BlueprintCallable)
	void NewGameButtonPressed(int32 Slot);

	// ���� ���� ��ư�� ������ ��
	UFUNCTION(BlueprintCallable)
	void SelectSlotButtonPressed(int32 Slot);

	// ���� ���� ��ư�� ������ ��
	UFUNCTION(BlueprintCallable)
	void DeleteButtonPressed();

	// ���� ��ư�� ������ ��
	UFUNCTION(BlueprintCallable)
	void PlayButtonPressed();

	// ��� ���� ���� �ε�
	void LoadData();

public:
	// ������ ���õ� �� �˷��� ��������Ʈ
	UPROPERTY(BlueprintAssignable)
	FSlotSelected SlotSelectedDelegate;

protected:
	// ��� �����ϱ� ���� �ʵ� �˸��� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 NumLoadSlot;

private:
	// �ε� ���� ��� Ŭ����
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadSlot> LoadSlotViewModelClass;

	// �ε� ���Ե�
	UPROPERTY()
	TMap<int32, UMVVM_LoadSlot*> LoadSlots;

	// ������ �ε� ����
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_0;
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_1;
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_2;

	// ���� �����ϰ� �ִ� ����
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> SelectedSlot;
};
