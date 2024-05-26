// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

class UAuraUserWidget;
class UOverlayWidgetController;
class UAttributeMenuWidgetController;
class USpellMenuWidgetController;
class UAbilitySystemComponent;
class UAttributeSet;
struct FWidgetControllerParams;

/**
 * 커스텀 HUD 클래스
 */
UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	// 위젯 컨트롤러 얻기(없으면 생성)
	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);
	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams);
	USpellMenuWidgetController* GetSpellMenuWidgetController(const FWidgetControllerParams& WCParams);

	// 오버레이 UI 생성과 오버레이 위젯 컨트롤러 생성 후 변수들 세팅
	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

private:
	// 기본 오버레이 UI
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass;
	UPROPERTY()
	TObjectPtr<UAuraUserWidget> OverlayWidget;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;
	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

	// 능력치 메뉴 UI
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;
	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;

	// 스킬 메뉴 UI
	UPROPERTY(EditAnywhere)
	TSubclassOf<USpellMenuWidgetController> SpellMenuWidgetControllerClass;
	UPROPERTY()
	TObjectPtr<USpellMenuWidgetController> SpellMenuWidgetController;
};
