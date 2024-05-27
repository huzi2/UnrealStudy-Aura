// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/SpellMenuWidgetController.h"

void USpellMenuWidgetController::BindCallbacksToDependencies()
{

}

void USpellMenuWidgetController::BroadcastInitialValue()
{
	// 기본 어빌리티 정보를 브로드캐스트해서 EquippedSpellRow의 UI를 갱신
	BroadcastAbilityInfo();
}
