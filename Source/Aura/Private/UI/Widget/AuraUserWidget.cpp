// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widget/AuraUserWidget.h"

void UAuraUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;

	// 아래 함수는 블루프린트에서 수행되며, 하위 위젯들에게 위젯 컨트롤러를 부여하고 값 변경에 대한 함수 바인드
	WidgetControllerSet();
}
