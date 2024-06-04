// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

// ��ų �޴����� ��ų Ʈ���� ��ų ��ư�� ������ �� ȣ���� ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSpellGlobeSelectedSignature, bool, bSpendPointButtonEnabled, bool, bEquipButtonEnabled);

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

public:
	// �÷��̾��� ��ų ����Ʈ�� ����Ǿ��� �� UI���� ����� ��������Ʈ. �������Ʈ���� ����
	UPROPERTY(BlueprintAssignable)
	FOnPlayerStatChangedSignature SpellPointsChangedDelegate;

	// ��ų �޴����� ��ų Ʈ���� ��ų ��ư�� ������ �� ȣ���� ��������Ʈ
	UPROPERTY(BlueprintAssignable)
	FSpellGlobeSelectedSignature SpellGlobeSelectedDelegate;
};
