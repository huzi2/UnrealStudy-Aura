// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ScalableFloat.h"
#include "CharacterClassInfo.generated.h"

class UGameplayEffect;
class UGameplayAbility;

/**
 * 캐릭터 클래스(직업) 종류
 */
UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	Elementalist,
	Warrior,
	Ranger
};

/**
 * 클래스별로 설정할 변수들
 */
USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()

public:
	// 클래스별 능력치
	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;

	// 클래스별 기본 어빌리티들
	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	// 클래스별 처치시 획득할 경험치
	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	FScalableFloat XPReward = FScalableFloat();
};

/**
 * 클래스별로 설정할 데이터 에셋 클래스
 */
UCLASS()
class AURA_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	const FCharacterClassDefaultInfo& GetClassDefaultInfo(ECharacterClass CharacterClass) const;

public:
	// 세이브 데이터로 세팅되는 기본 능력치
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes_SetByCaller;
	// 2차 속성과 체력마나 능력치는 모든 클래스가 공유할 것
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	// 세이브 게임에서 로드해서 능력치 적용할 때 사용할 지속시간 무한의 2차 속성
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes_Infinite;

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
