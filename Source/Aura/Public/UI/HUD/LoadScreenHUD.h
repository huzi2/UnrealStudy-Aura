// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LoadScreenHUD.generated.h"

class ULoadScreenWidget;
class UMVVM_LoadScreen;

/**
 * 로드 메뉴에서 사용할 HUD 클래스
 */
UCLASS()
class AURA_API ALoadScreenHUD : public AHUD
{
	GENERATED_BODY()
	
private:
	virtual void BeginPlay() final;

protected:
	// 로드 메뉴 UI 객체
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ULoadScreenWidget> LoadScreenWidget;

	// 로드 메뉴 뷰모델 객체
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMVVM_LoadScreen> LoadScreenViewModel;

private:
	// 로드 메뉴 UI 클래스
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> LoadScreenWidgetClass;

	// 로드 메뉴 뷰모델 클래스
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadScreen> LoadScreenViewModelClass;
};
