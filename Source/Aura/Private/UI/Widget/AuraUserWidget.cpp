// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widget/AuraUserWidget.h"

void UAuraUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;

	// �Ʒ� �Լ��� �������Ʈ���� ����Ǹ�, ���� �����鿡�� ���� ��Ʈ�ѷ��� �ο��ϰ� �� ���濡 ���� �Լ� ���ε�
	WidgetControllerSet();
}
