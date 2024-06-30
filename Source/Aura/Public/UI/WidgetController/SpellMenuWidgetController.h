// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AuraGameplayTags.h"
#include "SpellMenuWidgetController.generated.h"

// 스킬 메뉴에서 스킬 트리의 스킬 버튼을 눌렀을 때 호출할 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSpellGlobeSelectedSignature, bool, bSpendPointButtonEnabled, bool, bEquipButtonEnabled, FString, DescriptionString, FString, NextLevelDescriptionString);
// 스킬 장착 버튼을 눌렀을 때 호출할 델리게이트(애니메이션 캔슬)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquipSelectionSignature, const FGameplayTag&, AbilityTypeTag);
// 스킬 장착 버튼을 눌렀을 때 호출할 델리게이트(스킬 구슬의 대기상태 취소)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpellGlobeReassignedSignature, const FGameplayTag&, AbilityTag);

/**
* 스킬 메뉴의 위젯 컨트롤러
*/
USTRUCT()
struct FSelectedAbility
{
	GENERATED_BODY()

public:
	FGameplayTag AbilityTag = FGameplayTag();
	FGameplayTag StatusTag = FGameplayTag();
};

/**
 * 스킬 메뉴의 위젯 컨트롤러
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	virtual void BindCallbacksToDependencies() final;
	virtual void BroadcastInitialValue() final;

private:
	// 스킬 트리의 스킬 버튼이 눌렸음
	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelected(const FGameplayTag& AbilityTag);

	// 스킬 버튼 선택 취소
	UFUNCTION(BlueprintCallable)
	void SpellGlobeDeselect();

	// 스킬 포인트 버튼을 눌렀을 때
	UFUNCTION(BlueprintCallable)
	void SpendPointButtonPressed();

	// 스킬 장착 버튼을 눌렀을 때
	UFUNCTION(BlueprintCallable)
	void EquipButtonPressed();

	// 스킬 장착 대기 상태에서 장착할 스킬 구슬을 눌렀을 때
	UFUNCTION(BlueprintCallable)
	void SpellRowGlobePressed(const FGameplayTag& InputTag, const FGameplayTag& AbilityTypeTag);

	// 스킬 포인트와 장착 버튼의 활성화 유무 확인
	static void ShouldEnableButton(const FGameplayTag& StatusTag, int32 SpellPoints, bool& bShouldEnableSpellPointsButton, bool& bShouldEnableEquipButton);

	// 장착되는 스킬의 정보가 어빌리티 시스템 컴포넌트의 델리게이트를 통해 전달될 때 수행
	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& InputTag, const FGameplayTag& PrevInputTag);

public:
	// 플레이어의 스킬 포인트가 변경되었을 때 UI에서 사용할 델리게이트. 블루프린트에서 정의
	UPROPERTY(BlueprintAssignable)
	FOnPlayerStatChangedSignature SpellPointsChangedDelegate;

	// 스킬 메뉴에서 스킬 트리의 스킬 버튼을 눌렀을 때 호출할 델리게이트
	UPROPERTY(BlueprintAssignable)
	FSpellGlobeSelectedSignature SpellGlobeSelectedDelegate;

	// 스킬 장착 버튼을 눌렀을 때 호출할 델리게이트(애니메이션 캔슬)
	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature WaitForEquipSelectionDelegate;

	// 스킬 장착 버튼을 눌렀을 때 호출할 델리게이트(스킬 구슬의 대기상태 취소)
	UPROPERTY(BlueprintAssignable)
	FSpellGlobeReassignedSignature FSpellGlobeReassignedDelegate;

	// 스킬 장착 버튼을 누르고 대기 중에서 선택을 취소했을 때 호출할 델리게이트
	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature StopWaitingForEquipDelegate;

private:
	// 현재 선택한 스킬의 태그와 상태 태그
	FSelectedAbility SelectedAbility = { UAuraGameplayTags::Get().Abilities_None, UAuraGameplayTags::Get().Abilities_Status_Locked };
	// 현재 스킬 포인트
	int32 CurrentSpellPoints = 0;
	// 스킬 장착을 대기하는 상태인가?
	bool bWaitingForEquipSelection = false;
	// 현재 선택되어있는롯 스킬 슬롯의 인풋 태그(장착을 위함)
	FGameplayTag SelectedInputTag;
};
