// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameplayTagContainer.h"
#include "LoadScreenSaveGame.generated.h"

class UGameplayAbility;

// Ȱ��ȭ�� ���� ����
UENUM(BlueprintType)
enum ESaveSlotStatus
{
	Vacant,
	EnterName,
	Taken
};

// ���� ���¸� �����ϱ� ���� ����ü
USTRUCT()
struct FSavedActor
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FName ActorName = FName();

	UPROPERTY()
	FTransform Transform = FTransform();

	// ���Ϳ��� SaveGame���� ������ �������� ����ȭ�� �����ϴ� ����(FMemoryWriter���� ���)
	UPROPERTY()
	TArray<uint8> Bytes;
};

inline bool operator==(const FSavedActor& a, const FSavedActor& b)
{
	return a.ActorName == b.ActorName;
}

// �� ���¸� �����ϱ� ���� ����ü
USTRUCT()
struct FSavedMap
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString MapAssetName = FString();

	// �ʿ� �ִ� ��� ���͵� ����
	UPROPERTY()
	TArray<FSavedActor> SavedActors;
};

// �����Ƽ ������ ���� ����ü
USTRUCT(BlueprintType)
struct FSavedAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ClassDefaults")
	TSubclassOf<UGameplayAbility> GameplayAbility;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityTag = FGameplayTag();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityStatusTag = FGameplayTag();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityInputTag = FGameplayTag();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityTypeTag = FGameplayTag();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 AbilityLevel = 1;
};

// FSavedAbility�� ���ٴ� ����. AddUnique�� ���� �������
inline bool operator==(const FSavedAbility& a, const FSavedAbility& b)
{
	return a.AbilityTag.MatchesTagExact(b.AbilityTag);
}

/**
 * ������ ���� ������
 */
UCLASS()
class AURA_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	FSavedMap GetSavedMapWithMapName(const FString& InMapName) const;
	bool HasMap(const FString& InMapName) const;

public:
	// ���̺� ���� ���� ���� ����
	UPROPERTY()
	FString SlotName = FString();
	UPROPERTY()
	FString PlayerName = FString(TEXT("Default Nmae"));
	UPROPERTY()
	FString MapName = FString(TEXT("Default Map Nmae"));
	UPROPERTY()
	FString MapAssetName = FString(TEXT("Default Map Asset Nmae"));
	UPROPERTY()
	int32 SlotIndex = 0;
	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = ESaveSlotStatus::Vacant;
	UPROPERTY()
	FName PlayerStartTag;
	UPROPERTY()
	bool bFirstTimeLoadIn = true;

	// �÷��̾� ���� ���� ����
	UPROPERTY()
	int32 PlayerLevel = 1;
	UPROPERTY()
	int32 XP = 0;
	UPROPERTY()
	int32 AttributePoints = 0;
	UPROPERTY()
	int32 SpellPoints = 0;

	// �÷��̾� �⺻ �ɷ�ġ ���� ����
	UPROPERTY()
	float Strength = 0.f;
	UPROPERTY()
	float Intelligence = 0.f;
	UPROPERTY()
	float Resilience = 0.f;
	UPROPERTY()
	float Vigor = 0.f;

	// �÷��̾ ������ �ִ� ��ų��
	UPROPERTY()
	TArray<FSavedAbility> SavedAbilities;

	// ���ӿ� �ִ� ��� �� ����
	UPROPERTY()
	TArray<FSavedMap> SavedMaps;
};
