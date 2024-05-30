// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AbilityInfo.generated.h"

class UGameplayAbility;

/**
 * 캐릭터의 입력과 연결될 스킬
 */
USTRUCT(BlueprintType)
struct FAuraAbilityInfo
{
	GENERATED_BODY()

public:
	// 관련 태그
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilityTag = FGameplayTag();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag CooldownTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag InputTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag StatusTag = FGameplayTag();

	// UI에 표시될 아이콘과 배경
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UTexture2D> Icon = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UMaterialInterface> BackgroundMaterial = nullptr;

	// 스킬을 배우기 위한 필요 레벨
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 LevelRequirement = 1;

	// 스킬 어빌리티 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> Ability;
};

/**
 * 어빌리티와 어빌리티 정보를 연결하는 데이터 에셋 클래스
 */
UCLASS()
class AURA_API UAbilityInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	FORCEINLINE const TArray<FAuraAbilityInfo>& GetAbilityInformation() const { return AbilityInformation; }

	FAuraAbilityInfo FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bNotLogFound = false) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilityInformation")
	TArray<FAuraAbilityInfo> AbilityInformation;
};
