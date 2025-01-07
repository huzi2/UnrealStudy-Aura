// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/SaveInterface.h"
#include "AuraEnemySpawnVolume.generated.h"

class UBoxComponent;
class AAuraEnemySpawnPoint;

/**
 * �÷��̾ ������ ���� ��ȯ�ϴ� ���� Ŭ����
 */
UCLASS()
class AURA_API AAuraEnemySpawnVolume : public AActor, public ISaveInterface
{
	GENERATED_BODY()
	
private:
	AAuraEnemySpawnVolume();

private:
	virtual void BeginPlay() final;

	// ISaveInterface���� ���
	virtual bool ShouldLoadTransform_Implementation() const final { return false; };
	virtual void LoadActor_Implementation() final;

private:
	// �浹 ó��
	UFUNCTION()
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// �÷��̾ ��Ҿ��� �� ����ϱ����� ����
	UPROPERTY(BlueprintReadOnly, SaveGame)
	bool bReached = false;

private:
	// �浹 üũ Ȯ���� �ڽ� ������Ʈ
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> Box;

	UPROPERTY(EditAnywhere)
	TArray<AAuraEnemySpawnPoint*> SpawnPoints;
};
