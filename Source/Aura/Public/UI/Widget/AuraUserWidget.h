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
	// �� �Լ��� BlueprintImplementableEvent�� �����ؼ� ���� �������Ʈ���� Event Widget Controller Set �̺�Ʈ�� �����Ѵ�.
	// �ش� �Լ��� ���� �����鿡�� ������Ʈ�ѷ��� �ο��ϰ�, ���ε��� �Լ����� ���ε��Ѵ�.
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();

public:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;
};
