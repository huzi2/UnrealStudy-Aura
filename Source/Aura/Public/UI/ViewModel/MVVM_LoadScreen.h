// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadScreen.generated.h"

class UMVVM_LoadSlot;

// 슬롯이 선택될 때 알려줄 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSlotSelected);

/**
 * 로드 메뉴와 연결된 뷰모델 클래스
 */
UCLASS()
class AURA_API UMVVM_LoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	// 로드 슬롯 뷰모델들 생성
	void InitializeLoadSlot();

	// 로드 슬롯 가져오기
	UFUNCTION(BlueprintPure)
	UMVVM_LoadSlot* GetLoadSlotViewModelByIndex(int32 Index) const;

	// 새 슬롯 버튼을 눌렀을 때
	UFUNCTION(BlueprintCallable)
	void NewSlotButtonPressed(int32 Slot, const FString& EnteredName);

	// 새 게임 버튼을 눌렀을 때
	UFUNCTION(BlueprintCallable)
	void NewGameButtonPressed(int32 Slot);

	// 슬롯 선택 버튼을 눌렀을 때
	UFUNCTION(BlueprintCallable)
	void SelectSlotButtonPressed(int32 Slot);

	// 모든 슬롯 내용 로드
	void LoadData();

public:
	// 슬롯이 선택될 때 알려줄 델리게이트
	UPROPERTY(BlueprintAssignable)
	FSlotSelected SlotSelectedDelegate;

private:
	// 로드 슬롯 뷰모델 클래스
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadSlot> LoadSlotViewModelClass;

	// 로드 슬롯들
	UPROPERTY()
	TMap<int32, UMVVM_LoadSlot*> LoadSlots;

	// 각각의 로드 슬롯
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_0;
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_1;
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_2;
};
