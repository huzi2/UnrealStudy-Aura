// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameplayTagContainer.h"
#include "LoadScreenSaveGame.generated.h"

class UGameplayAbility;

// 활성화된 슬롯 종류
UENUM(BlueprintType)
enum ESaveSlotStatus
{
	Vacant,
	EnterName,
	Taken
};

// 액터 상태를 저장하기 위한 구조체
USTRUCT()
struct FSavedActor
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FName ActorName = FName();

	UPROPERTY()
	FTransform Transform = FTransform();

	// 액터에서 SaveGame으로 설정된 변수들을 직렬화로 저장하는 변수(FMemoryWriter에서 사용)
	UPROPERTY()
	TArray<uint8> Bytes;
};

inline bool operator==(const FSavedActor& a, const FSavedActor& b)
{
	return a.ActorName == b.ActorName;
}

// 맵 상태를 저장하기 위한 구조체
USTRUCT()
struct FSavedMap
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString MapAssetName = FString();

	// 맵에 있던 모든 액터들 정보
	UPROPERTY()
	TArray<FSavedActor> SavedActors;
};

// 어빌리티 저장을 위한 구조체
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

// FSavedAbility가 같다는 조건. AddUnique를 위해 만들었음
inline bool operator==(const FSavedAbility& a, const FSavedAbility& b)
{
	return a.AbilityTag.MatchesTagExact(b.AbilityTag);
}

/**
 * 저장할 게임 데이터
 */
UCLASS()
class AURA_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	FSavedMap GetSavedMapWithMapName(const FString& InMapName) const;
	bool HasMap(const FString& InMapName) const;

public:
	// 세이브 슬롯 관련 저장 내용
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

	// 플레이어 관련 저장 내용
	UPROPERTY()
	int32 PlayerLevel = 1;
	UPROPERTY()
	int32 XP = 0;
	UPROPERTY()
	int32 AttributePoints = 0;
	UPROPERTY()
	int32 SpellPoints = 0;

	// 플레이어 기본 능력치 저장 내용
	UPROPERTY()
	float Strength = 0.f;
	UPROPERTY()
	float Intelligence = 0.f;
	UPROPERTY()
	float Resilience = 0.f;
	UPROPERTY()
	float Vigor = 0.f;

	// 플레이어가 가지고 있던 스킬들
	UPROPERTY()
	TArray<FSavedAbility> SavedAbilities;

	// 게임에 있던 모든 맵 정보
	UPROPERTY()
	TArray<FSavedMap> SavedMaps;
};
