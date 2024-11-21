// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/AuraProjectile.h"
#include "AuraFireBall.generated.h"

/**
 * 화염 폭발에서 사용할 발사체 클래스
 */
UCLASS()
class AURA_API AAuraFireBall : public AAuraProjectile
{
	GENERATED_BODY()

private:
	virtual void BeginPlay() final;

	// AAuraProjectile에서 상속
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) final;
};
