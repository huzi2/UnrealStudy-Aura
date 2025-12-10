// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AuraInputConfig.generated.h"

class UInputAction;

/**
 * 향상된 입력과 태그를 연결하기 위한 구조체
 */
USTRUCT(BlueprintType)
struct FAuraInputAction
{
	GENERATED_BODY()

public:
	// 인풋 액션
	UPROPERTY(EditDefaultsOnly)
	const UInputAction* InputAction = nullptr;
	// 인풋 태그
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag = FGameplayTag();
};

/**
 * 인풋 태그와 액션 관리 클래스
 */
UCLASS()
class AURA_API UAuraInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// 인풋 태그로 인풋 액션 찾기
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = false) const;

public:
	// 모든 인풋 태그-액션 연결 컨테이너
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAuraInputAction> AbilityInputActions;
};
