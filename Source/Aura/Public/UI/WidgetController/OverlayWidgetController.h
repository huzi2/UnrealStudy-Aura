// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "GameplayTagContainer.h"
#include "OverlayWidgetController.generated.h"

class UAuraUserWidget;
class UAuraAbilitySystemComponent;
struct FOnAttributeChangeData;

/**
 * 메시지 UI에서 사용할 구조체
 */
USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase
{
	// 이 테이블의 키 값은 FName
	GENERATED_BODY()

public:
	// 메시지의 태그
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();
	// 메시지 내용
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message = FText();
	// 메시지 UI 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UAuraUserWidget> MessageWidget;
	// 메시지에 표시할 이미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Image = nullptr;
};

// 어트리뷰트 세트의 값이 변경될 때 위젯들에게 알려줄 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);
// 레벨이 변경되었을 때 위젯들에게 알려줄 델리게이트. bool 값은 알림을 띄울지 여부
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLevelChangedSignature, int32, NewValue, bool, bLevelUp);
// 태그에 따른 메시지 위젯을 표시할 때 사용할 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature, FUIWidgetRow, Row);

/**
 * 오버레이 UI와 코드(블루프린트)를 연결해주는 컨트롤러
 */
UCLASS(BlueprintType, Blueprintable) // 블루프린트에서 직접 사용하기 위해 속성 지정
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	virtual void BindCallbacksToDependencies() final;
	virtual void BroadcastInitialValue() final;

protected:
	// 장착되는 스킬의 정보가 어빌리티 시스템 컴포넌트의 델리게이트를 통해 전달될 때 수행
	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& InputTag, const FGameplayTag& PrevInputTag) const;

private:
	// 테이블에서 FName으로 사용해서 필요한 데이터를 얻어옴. 에디터에서 테이블을 만들 때 키값을 태그와 똑같이 만들어야한다.
	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag) const;

	// 경험치 변경 시 UI에 적용
	void OnXPChanged(int32 NewXP);

public:
	// BlueprintAssignable로 연결되는 함수들은 각 UI의 블루프린트에서 직접 구현한다.
	// 어트리뷰트 값들이 변경될 때 사용할 델리게이트들
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnManaChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxManaChanged;

	// 메시지 출력과 연결할 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FMessageWidgetRowSignature MessageWidgetRowDelegate;

	// 경험치가 변경될 때 사용할 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "GAS|XP")
	FOnAttributeChangedSignature OnXPPercentChangedDelegate;

	// 레벨 업 시 레벨을 수정할 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "GAS|Level")
	FOnLevelChangedSignature OnPlayerLevelChangedDelegate;

protected:
	// 메시지 관련 데이터 테이블
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;
};

template<typename T>
inline T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag) const
{
	if (!DataTable) return nullptr;
	
	return DataTable->FindRow<T>(Tag.GetTagName(), FString());
}
