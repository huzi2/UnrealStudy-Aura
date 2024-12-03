// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/HUD/LoadScreenHUD.h"
#include "UI/Widget/LoadScreenWidget.h"
#include "UI/ViewModel/MVVM_LoadScreen.h"

void ALoadScreenHUD::BeginPlay()
{
	Super::BeginPlay();

	LoadScreenViewModel = NewObject<UMVVM_LoadScreen>(this, LoadScreenViewModelClass);

	if (LoadScreenWidget = CreateWidget<ULoadScreenWidget>(GetWorld(), LoadScreenWidgetClass))
	{
		LoadScreenWidget->AddToViewport();
	}
}
