// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadSlot::SetPlayerName(const FString& InPlayerName)
{
	// 뷰모델을 통해서 변수가 바뀌었음을 알림(내부에서 브로드캐스트)
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, InPlayerName);
}

void UMVVM_LoadSlot::SetMapName(const FString& InMapName)
{
	UE_MVVM_SET_PROPERTY_VALUE(MapName, InMapName);
}

void UMVVM_LoadSlot::SetSlotName(const FString& InSlotName)
{
	UE_MVVM_SET_PROPERTY_VALUE(SlotName, InSlotName);
}

void UMVVM_LoadSlot::SetPlayerLevel(int32 InPlayerLevel)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerLevel, InPlayerLevel);
}

void UMVVM_LoadSlot::InitializeSlot()
{
	const int32 WidgetSwitcherIndex = SlotStatus.GetValue();
	SetWidgetSwitcherIndexDelegate.Broadcast(WidgetSwitcherIndex);
}
