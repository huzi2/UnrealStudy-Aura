// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AuraGameplayTags.h"
#include "SpellMenuWidgetController.generated.h"

// ��ų �޴����� ��ų Ʈ���� ��ų ��ư�� ������ �� ȣ���� ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSpellGlobeSelectedSignature, bool, bSpendPointButtonEnabled, bool, bEquipButtonEnabled, FString, DescriptionString, FString, NextLevelDescriptionString);
// ��ų ���� ��ư�� ������ �� ȣ���� ��������Ʈ(�ִϸ��̼� ĵ��)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquipSelectionSignature, const FGameplayTag&, AbilityTypeTag);
// ��ų ���� ��ư�� ������ �� ȣ���� ��������Ʈ(��ų ������ ������ ���)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpellGlobeReassignedSignature, const FGameplayTag&, AbilityTag);

/**
* ��ų �޴��� ���� ��Ʈ�ѷ�
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
 * ��ų �޴��� ���� ��Ʈ�ѷ�
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	virtual void BindCallbacksToDependencies() final;
	virtual void BroadcastInitialValue() final;

private:
	// ��ų Ʈ���� ��ų ��ư�� ������
	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelected(const FGameplayTag& AbilityTag);

	// ��ų ��ư ���� ���
	UFUNCTION(BlueprintCallable)
	void SpellGlobeDeselect();

	// ��ų ����Ʈ ��ư�� ������ ��
	UFUNCTION(BlueprintCallable)
	void SpendPointButtonPressed();

	// ��ų ���� ��ư�� ������ ��
	UFUNCTION(BlueprintCallable)
	void EquipButtonPressed();

	// ��ų ���� ��� ���¿��� ������ ��ų ������ ������ ��
	UFUNCTION(BlueprintCallable)
	void SpellRowGlobePressed(const FGameplayTag& InputTag, const FGameplayTag& AbilityTypeTag);

	// ��ų ����Ʈ�� ���� ��ư�� Ȱ��ȭ ���� Ȯ��
	static void ShouldEnableButton(const FGameplayTag& StatusTag, int32 SpellPoints, bool& bShouldEnableSpellPointsButton, bool& bShouldEnableEquipButton);

	// �����Ǵ� ��ų�� ������ �����Ƽ �ý��� ������Ʈ�� ��������Ʈ�� ���� ���޵� �� ����
	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& InputTag, const FGameplayTag& PrevInputTag);

public:
	// �÷��̾��� ��ų ����Ʈ�� ����Ǿ��� �� UI���� ����� ��������Ʈ. �������Ʈ���� ����
	UPROPERTY(BlueprintAssignable)
	FOnPlayerStatChangedSignature SpellPointsChangedDelegate;

	// ��ų �޴����� ��ų Ʈ���� ��ų ��ư�� ������ �� ȣ���� ��������Ʈ
	UPROPERTY(BlueprintAssignable)
	FSpellGlobeSelectedSignature SpellGlobeSelectedDelegate;

	// ��ų ���� ��ư�� ������ �� ȣ���� ��������Ʈ(�ִϸ��̼� ĵ��)
	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature WaitForEquipSelectionDelegate;

	// ��ų ���� ��ư�� ������ �� ȣ���� ��������Ʈ(��ų ������ ������ ���)
	UPROPERTY(BlueprintAssignable)
	FSpellGlobeReassignedSignature FSpellGlobeReassignedDelegate;

	// ��ų ���� ��ư�� ������ ��� �߿��� ������ ������� �� ȣ���� ��������Ʈ
	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature StopWaitingForEquipDelegate;

private:
	// ���� ������ ��ų�� �±׿� ���� �±�
	FSelectedAbility SelectedAbility = { UAuraGameplayTags::Get().Abilities_None, UAuraGameplayTags::Get().Abilities_Status_Locked };
	// ���� ��ų ����Ʈ
	int32 CurrentSpellPoints = 0;
	// ��ų ������ ����ϴ� �����ΰ�?
	bool bWaitingForEquipSelection = false;
	// ���� ���õǾ��ִ·� ��ų ������ ��ǲ �±�(������ ����)
	FGameplayTag SelectedInputTag;
};
