// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MagicCircle.generated.h"

/**
 * 마법진 클래스
 */
UCLASS()
class AURA_API AMagicCircle : public AActor
{
	GENERATED_BODY()
	
private:
	AMagicCircle();

private:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	// 마법진에 표시할 데칼 이펙트
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDecalComponent> MagicCircleDecal;
};
