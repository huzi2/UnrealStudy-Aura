// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LootTiers.generated.h"

USTRUCT(BlueprintType)
struct FLootItem
{
	GENERATED_BODY()

public:
	// ����� ��ü Ŭ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LootTiers|Spawning")
	TSubclassOf<AActor> LootClass;

	// ��� Ȯ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LootTiers|Spawning")
	float ChanceToSpawn = 0.f;

	// �ִ� ��� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LootTiers|Spawning")
	int32 MaxNumberToSpawn = 0;

	// ��� ��ü�� ������ ������ ���ΰ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LootTiers|Spawning")
	bool bLootLevelOverride = true;
};

/**
 * 
 */
UCLASS()
class AURA_API ULootTiers : public UDataAsset
{
	GENERATED_BODY()

public:
	// Ȯ���� ������ ����ؼ� ����� �����۵��� ����
	UFUNCTION(BlueprintPure)
	TArray<FLootItem> GeLootItems() const;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LootTiers|Spawning")
	TArray<FLootItem> LootItems;
};
