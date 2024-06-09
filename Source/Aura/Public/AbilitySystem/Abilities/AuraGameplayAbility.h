// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

/**
 * 커스텀 어빌리티 클래스
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	// 레벨에 따른 어빌리티의 설명 얻기
	virtual FString GetDescription(int32 Level) const;
	virtual FString GetNextLevelDescription(int32 Level) const;
	static FString GetLockedDescription(int32 Level);

public:
	// 어빌리티의 입력 관련 태그
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FGameplayTag StartupInputTag;
};
