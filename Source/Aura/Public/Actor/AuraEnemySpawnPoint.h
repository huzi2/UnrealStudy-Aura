// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AuraEnemySpawnPoint.generated.h"

class AAuraEnemy;

/**
 * 적 객체를 소환하는 스폰 포인트 객체
 */
UCLASS()
class AURA_API AAuraEnemySpawnPoint : public ATargetPoint
{
	GENERATED_BODY()

public:
	// 적 소환
	UFUNCTION(BlueprintCallable)
	void SpawnEnemy() const;

protected:
	// 적 객체 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Class")
	TSubclassOf<AAuraEnemy> EnemyClass;
	
	// 소환될 적의 레벨
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Class")
	int32 EnemyLevel = 1;

	// 소환될 적의 타입
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Class")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;
};
