// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AuraEnemySpawnPoint.generated.h"

class AAuraEnemy;

/**
 * �� ��ü�� ��ȯ�ϴ� ���� ����Ʈ ��ü
 */
UCLASS()
class AURA_API AAuraEnemySpawnPoint : public ATargetPoint
{
	GENERATED_BODY()

public:
	// �� ��ȯ
	UFUNCTION(BlueprintCallable)
	void SpawnEnemy() const;

protected:
	// �� ��ü Ŭ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Class")
	TSubclassOf<AAuraEnemy> EnemyClass;
	
	// ��ȯ�� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Class")
	int32 EnemyLevel = 1;

	// ��ȯ�� ���� Ÿ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Class")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;
};
