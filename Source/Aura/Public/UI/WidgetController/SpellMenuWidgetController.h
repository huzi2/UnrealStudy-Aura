// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AuraGameplayTags.h"
#include "SpellMenuWidgetController.generated.h"

// ��ų �޴����� ��ų Ʈ���� ��ų ��ư�� ������ �� ȣ���� ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSpellGlobeSelectedSignature, bool, bSpendPointButtonEnabled, bool, bEquipButtonEnabled);

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

	// ��ų ����Ʈ ��ư�� ������ ��
	UFUNCTION(BlueprintCallable)
	void SpendPointButtonPressed();

	// ��ų ����Ʈ�� ���� ��ư�� Ȱ��ȭ ���� Ȯ��
	static void ShouldEnableButton(const FGameplayTag& StatusTag, int32 SpellPoints, bool& bShouldEnableSpellPointsButton, bool& bShouldEnableEquipButton);

public:
	// �÷��̾��� ��ų ����Ʈ�� ����Ǿ��� �� UI���� ����� ��������Ʈ. �������Ʈ���� ����
	UPROPERTY(BlueprintAssignable)
	FOnPlayerStatChangedSignature SpellPointsChangedDelegate;

	// ��ų �޴����� ��ų Ʈ���� ��ų ��ư�� ������ �� ȣ���� ��������Ʈ
	UPROPERTY(BlueprintAssignable)
	FSpellGlobeSelectedSignature SpellGlobeSelectedDelegate;

private:
	// ���� ������ ��ų�� �±׿� ���� �±�
	FSelectedAbility SelectedAbility = { UAuraGameplayTags::Get().Abilities_None, UAuraGameplayTags::Get().Abilities_Status_Locked };
	// ���� ��ų ����Ʈ
	int32 CurrentSpellPoints = 0;
};
