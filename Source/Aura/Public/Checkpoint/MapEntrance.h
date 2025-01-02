// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Checkpoint/Checkpoint.h"
#include "MapEntrance.generated.h"

/**
 * ���� �Ա� ��ü Ŭ����
 */
UCLASS()
class AURA_API AMapEntrance : public ACheckpoint
{
	GENERATED_BODY()
	
private:
	// ACheckpoint���� ���
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) final;

	// ISaveInterface���� ���
	virtual void LoadActor_Implementation() final;

	// IHighlightInterface���� ���
	virtual void HighlightActor_Implementation() final;

private:
	// �Ѿ ��
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> DestinationMap;

	// �ʿ��� ������ �÷��̾� ��ŸƮ ��ü�� �±�
	UPROPERTY(EditAnywhere)
	FName DestinationPlayerStartTag;
};
