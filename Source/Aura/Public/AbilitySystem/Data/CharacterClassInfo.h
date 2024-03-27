// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

class UGameplayEffect;
class UGameplayAbility;

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	Elementalist,
	Warrior,
	Ranger
};


USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;

	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
};

/**
 * 
 */
UCLASS()
class AURA_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	FCharacterClassDefaultInfo GetClassDefaultInfo(ECharacterClass CharacterClass) const;

public:
	// 2차 속성과 체력마나 능력치는 모든 클래스가 공유할 것
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;

	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	// 데미지 계산에 사용될 계수 커브
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults|Damage")
	TObjectPtr<UCurveTable> DamageCalculationCoefficients;

	// 주요 속성은 각 클래스별로 다르게 설정
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInformation;

	// 모든 캐릭터가 공통적으로 가지는 어빌리티들
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;
};
