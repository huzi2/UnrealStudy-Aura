// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "GameplayTagContainer.h"
#include "OverlayWidgetController.generated.h"

class UAuraUserWidget;
class UAbilityInfo;
class UAuraAbilitySystemComponent;
struct FOnAttributeChangeData;

/**
 * �޽��� UI���� ����� ����ü
 */
USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase
{
	// �� ���̺��� Ű ���� FName
	GENERATED_BODY()

public:
	// �޽����� �±�
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();
	// �޽��� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message = FText();
	// �޽��� UI Ŭ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UAuraUserWidget> MessageWidget;
	// �޽����� ǥ���� �̹���
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Image = nullptr;
};

// �÷��̾��� ���� ����(����, ��ų����Ʈ ��)�� ������� �� �˷��� ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, int32, NewValue);
// ��Ʈ����Ʈ ��Ʈ�� ���� ����� �� �����鿡�� �˷��� ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);
// �±׿� ���� �޽��� ������ ǥ���� �� ����� ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature, FUIWidgetRow, Row);
// �����Ƽ ������ Ȯ���ؼ� UI�� ǥ���ϱ����� ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FAuraAbilityInfo&, Info);

/**
 * �������� UI�� �ڵ�(�������Ʈ)�� �������ִ� ��Ʈ�ѷ�
 */
UCLASS(BlueprintType, Blueprintable) // �������Ʈ���� ���� ����ϱ� ���� �Ӽ� ����
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	virtual void BroadcastInitialValue() final;
	virtual void BindCallbacksToDependencies() final;

private:
	// ���̺��� FName���� ����ؼ� �ʿ��� �����͸� ����. �����Ϳ��� ���̺��� ���� �� Ű���� �±׿� �Ȱ��� �������Ѵ�.
	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag) const;

	// ���� �����Ƽ���� UI�� ����
	void OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraAbilitySystemComponent);
	// ����ġ ���� �� UI�� ����
	void OnXPChanged(int32 NewXP) const;

public:
	// BlueprintAssignable�� ����Ǵ� �Լ����� �� UI�� �������Ʈ���� ���� �����Ѵ�.
	// ��Ʈ����Ʈ ������ ����� �� ����� ��������Ʈ��
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnManaChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxManaChanged;

	// �޽��� ��°� ������ ��������Ʈ
	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FMessageWidgetRowSignature MessageWidgetRowDelegate;

	// �����Ƽ ������ ������ ��������Ʈ
	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FAbilityInfoSignature AbilityInfoDelegate;

	// ����ġ�� ����� �� ����� ��������Ʈ
	UPROPERTY(BlueprintAssignable, Category = "GAS|XP")
	FOnAttributeChangedSignature OnXPPercentChangedDelegate;

	// ���� �� �� ������ ������ ��������Ʈ
	UPROPERTY(BlueprintAssignable, Category = "GAS|Level")
	FOnPlayerStatChangedSignature OnPlayerLevelChangedDelegate;

protected:
	// �޽��� ���� ������ ���̺�
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UAbilityInfo> AbilityInfo;
};

template<typename T>
inline T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag) const
{
	if (!DataTable) return nullptr;
	
	return DataTable->FindRow<T>(Tag.GetTagName(), FString());
}
