// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/SaveInterface.h"
#include "AuraEnemySpawnVolume.generated.h"

class UBoxComponent;
class AAuraEnemySpawnPoint;

/**
 * 플레이어가 닿으면 적을 소환하는 볼륨 클래스
 */
UCLASS()
class AURA_API AAuraEnemySpawnVolume : public AActor, public ISaveInterface
{
	GENERATED_BODY()
	
private:
	AAuraEnemySpawnVolume();

private:
	virtual void BeginPlay() final;

	// ISaveInterface에서 상속
	virtual bool ShouldLoadTransform_Implementation() const final { return false; };
	virtual void LoadActor_Implementation() final;

private:
	// 충돌 처리
	UFUNCTION()
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// 플레이어가 닿았었는 지 기억하기위해 저장
	UPROPERTY(BlueprintReadOnly, SaveGame)
	bool bReached = false;

private:
	// 충돌 체크 확인할 박스 컴포넌트
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> Box;

	UPROPERTY(EditAnywhere)
	TArray<AAuraEnemySpawnPoint*> SpawnPoints;
};
