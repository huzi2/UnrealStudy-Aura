// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Checkpoint/Checkpoint.h"
#include "MapEntrance.generated.h"

/**
 * 맵의 입구 객체 클래스
 */
UCLASS()
class AURA_API AMapEntrance : public ACheckpoint
{
	GENERATED_BODY()
	
private:
	// ACheckpoint에서 상속
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) final;

	// ISaveInterface에서 상속
	virtual void LoadActor_Implementation() final;

	// IHighlightInterface에서 상속
	virtual void HighlightActor_Implementation() final;

private:
	// 넘어갈 맵
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> DestinationMap;

	// 맵에서 시작할 플레이어 스타트 객체의 태그
	UPROPERTY(EditAnywhere)
	FName DestinationPlayerStartTag;
};
