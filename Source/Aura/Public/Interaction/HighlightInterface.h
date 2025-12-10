// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HighlightInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UHighlightInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 하이라이트를 적용할 인터페이스
 */
class AURA_API IHighlightInterface
{
	GENERATED_BODY()

public:
	// 하이라이트
	UFUNCTION(BlueprintNativeEvent)
	void HighlightActor();
	UFUNCTION(BlueprintNativeEvent)
	void UnHighlightActor();

	// 하이라이트된 대상에게서 이동 지점 얻어오기
	UFUNCTION(BlueprintNativeEvent)
	void SetMoveToLocation(FVector& OutDestination);
};
