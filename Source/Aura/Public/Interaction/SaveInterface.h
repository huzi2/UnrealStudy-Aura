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
 * 액터를 저장, 로드할 때 사용할 인터페이스
 */
class AURA_API ISaveInterface
{
	GENERATED_BODY()

public:
	// 트랜스폼을 로드할 필요가 있는 객체인가?
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool ShouldLoadTransform() const;

	// 로드 후 수생할 함수
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void LoadActor();
};
