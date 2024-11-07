// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PointCollection.generated.h"

/**
 * Ư�� ����Ʈ �������� �����ϴ� ����(���� ������ ���� ������ ��)
 */
UCLASS()
class AURA_API APointCollection : public AActor
{
	GENERATED_BODY()
	
private:
	APointCollection();

private:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintPure)
	TArray<USceneComponent*> GetGroundPoints(const FVector& GroundLocation, int32 NumPoints, float YawOverride = 0.f) const;

protected:
	// ����Ʈ �������� ���� �迭
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<USceneComponent*> ImmutablePts;

	// ����Ʈ ������
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Pt_0;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Pt_1;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Pt_2;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Pt_3;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Pt_4;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Pt_5;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Pt_6;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Pt_7;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Pt_8;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Pt_9;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> Pt_10;
};
