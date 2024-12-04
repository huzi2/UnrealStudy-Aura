// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadScreenWidget.generated.h"

/**
 * 로드 메뉴 UI 클래스
 */
UCLASS()
class AURA_API ULoadScreenWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BlueprintInitializeWidget();
};
