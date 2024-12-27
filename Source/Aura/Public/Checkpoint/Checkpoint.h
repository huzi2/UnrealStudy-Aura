// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/SaveInterface.h"
#include "Checkpoint.generated.h"

class USphereComponent;

/**
 * üũ����Ʈ ��ü Ŭ����
 */
UCLASS()
class AURA_API ACheckpoint : public APlayerStart, public ISaveInterface
{
	GENERATED_BODY()

private:
	ACheckpoint(const FObjectInitializer& ObjectInitializer);

private:
	virtual void BeginPlay() final;

public:
	bool IsReached() const { return bReached; }
	void SetIsReached(bool bInReached) { bReached = bInReached; }

protected:
	// �������Ʈ���� ��Ƽ����� Ÿ�Ӷ����� �����ϱ����� �Լ�
	UFUNCTION(BlueprintImplementableEvent)
	void CheckpointReached(UMaterialInstanceDynamic* DynamicMaterialInstance);

private:
	// �浹 ó��
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// üũ����Ʈ�� ĳ���Ͱ� ����� �� �޽��� ��Ƽ���� ����
	void HandleGlowEffects();

protected:
	UPROPERTY(BlueprintReadOnly, SaveGame)
	bool bReached = false;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> CheckpointMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;
};
