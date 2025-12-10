// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);

protected:
	// 이 함수를 BlueprintImplementableEvent로 지정해서 실제 블루프린트에서 Event Widget Controller Set 이벤트로 구현한다.
	// 해당 함수로 가장 위젯들에게 위젯컨트롤러를 부여하고, 바인드할 함수들을 바인드한다.
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();

public:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;
};
