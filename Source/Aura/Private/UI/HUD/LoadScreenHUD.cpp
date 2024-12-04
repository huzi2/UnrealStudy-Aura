// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/HUD/LoadScreenHUD.h"
#include "UI/Widget/LoadScreenWidget.h"
#include "UI/ViewModel/MVVM_LoadScreen.h"

void ALoadScreenHUD::BeginPlay()
{
	Super::BeginPlay();

	if (!LoadScreenViewModelClass) return;
	if (!LoadScreenWidgetClass) return;

	if (LoadScreenViewModel = NewObject<UMVVM_LoadScreen>(this, LoadScreenViewModelClass))
	{
		LoadScreenViewModel->InitializeLoadSlot();
	}

	if (LoadScreenWidget = CreateWidget<ULoadScreenWidget>(GetWorld(), LoadScreenWidgetClass))
	{
		LoadScreenWidget->AddToViewport();
		LoadScreenWidget->BlueprintInitializeWidget();
	}
}
