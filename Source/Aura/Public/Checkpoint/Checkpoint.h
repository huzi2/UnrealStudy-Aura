// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/SaveInterface.h"
#include "Checkpoint.generated.h"

class USphereComponent;

/**
 * 체크포인트 객체 클래스
 */
UCLASS()
class AURA_API ACheckpoint : public APlayerStart, public ISaveInterface
{
	GENERATED_BODY()

private:
	ACheckpoint(const FObjectInitializer& ObjectInitializer);

private:
	virtual void BeginPlay() final;

	// ISaveInterface에서 상속
	virtual bool ShouldLoadTransform_Implementation() const override { return false; };
	virtual void LoadActor_Implementation() override;

public:
	bool IsReached() const { return bReached; }
	void SetIsReached(bool bInReached) { bReached = bInReached; }

protected:
	// 블루프린트에서 머티리얼과 타임라인을 연결하기위한 함수
	UFUNCTION(BlueprintImplementableEvent)
	void CheckpointReached(UMaterialInstanceDynamic* DynamicMaterialInstance);

private:
	// 충돌 처리
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 체크포인트에 캐릭터가 닿았을 때 메쉬의 머티리얼 변경
	void HandleGlowEffects();

protected:
	UPROPERTY(BlueprintReadOnly, SaveGame)
	bool bReached = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> CheckpointMesh;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;
};
