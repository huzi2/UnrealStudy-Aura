// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AuraGameplayTags.h"

void UAttributeMenuWidgetController::BroadcastInitialValue()
{
	check(AttributeInfo);
	const UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	
	// 데이터에셋을 사용해서 에셋들에게 초기값을 브로드캐스트
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(UAuraGameplayTags::Get().Attribute_Primary_Strength);
	Info.AttributeValue = AS->GetStrength();
	AttributeInfoDelegate.Broadcast(Info);
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	
}
