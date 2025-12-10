// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/HUD/LoadScreenHUD.h"
#include "UI/Widget/LoadScreenWidget.h"
#include "UI/ViewModel/MVVM_LoadScreen.h"

void ALoadScreenHUD::BeginPlay()
{
	Super::BeginPlay();

	if (!LoadScreenViewModelClass) return;
	if (!LoadScreenWidgetClass) return;

	LoadScreenViewModel = NewObject<UMVVM_LoadScreen>(this, LoadScreenViewModelClass);
	if (!LoadScreenViewModel) return;

	LoadScreenWidget = CreateWidget<ULoadScreenWidget>(GetWorld(), LoadScreenWidgetClass);
	if (!LoadScreenWidget) return;

	LoadScreenViewModel->InitializeLoadSlot();
	LoadScreenWidget->AddToViewport();
	LoadScreenWidget->BlueprintInitializeWidget();

	// 모든 슬롯에 저장된 값들 세팅
	LoadScreenViewModel->LoadData();
}
