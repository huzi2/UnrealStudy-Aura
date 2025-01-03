// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/SaveInterface.h"
#include "Interaction/HighlightInterface.h"
#include "Aura/Aura.h"
#include "Checkpoint.generated.h"

class USphereComponent;

/**
 * üũ����Ʈ ��ü Ŭ����
 */
UCLASS()
class AURA_API ACheckpoint : public APlayerStart, public ISaveInterface, public IHighlightInterface
{
	GENERATED_BODY()

protected:
	ACheckpoint(const FObjectInitializer& ObjectInitializer);

private:
	virtual void BeginPlay() override;

	// ISaveInterface���� ���
	virtual bool ShouldLoadTransform_Implementation() const override { return false; };
	virtual void LoadActor_Implementation() override;

	// IHighlightInterface���� ���
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	virtual void SetMoveToLocation_Implementation(FVector& OutDestination) override;

protected:
	// �������Ʈ���� ��Ƽ����� Ÿ�Ӷ����� �����ϱ����� �Լ�
	UFUNCTION(BlueprintImplementableEvent)
	void CheckpointReached(UMaterialInstanceDynamic* DynamicMaterialInstance);

private:
	// üũ����Ʈ�� ĳ���Ͱ� ����� �� �޽��� ��Ƽ���� ����
	UFUNCTION(BlueprintCallable)
	void HandleGlowEffects();

	// �浹 ó��
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// �÷��̾ �ش� üũ ����Ʈ�� ��Ҿ��°�?
	UPROPERTY(BlueprintReadWrite, SaveGame)
	bool bReached = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> CheckpointMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

	// ���̶���Ʈ�Ǿ��� �� �̵� �ൿ���� �� �̵��� ��ġ
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> MoveToComponent;

private:
	// ���̶���Ʈ���� ������ ����
	UPROPERTY(EditDefaultsOnly)
	int32 CustomDepthStencilOverride = CUSTOM_DEPTH_TAN;

	// ���� ���� �����ϴ� ����� �������?
	UPROPERTY(EditAnywhere)
	bool bBindOverlapCallback = true;
};
