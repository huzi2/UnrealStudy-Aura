// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SaveInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USaveInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * ���͸� ����, �ε��� �� ����� �������̽�
 */
class AURA_API ISaveInterface
{
	GENERATED_BODY()

public:
	// Ʈ�������� �ε��� �ʿ䰡 �ִ� ��ü�ΰ�?
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool ShouldLoadTransform() const;

	// �ε� �� ������ �Լ�
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void LoadActor();
};
