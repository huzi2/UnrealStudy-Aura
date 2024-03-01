// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AuraGameplayTags.h"

void UAttributeMenuWidgetController::BroadcastInitialValue()
{
	check(AttributeInfo);
	const UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	
	// �����Ϳ����� ����ؼ� ���µ鿡�� �ʱⰪ�� ��ε�ĳ��Ʈ
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(UAuraGameplayTags::Get().Attribute_Primary_Strength);
	Info.AttributeValue = AS->GetStrength();
	AttributeInfoDelegate.Broadcast(Info);
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	
}
