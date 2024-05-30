// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class UCharacterClassInfo;
class UAbilityInfo;

/**
 * 게임모드 클래스
 */
UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	FORCEINLINE UCharacterClassInfo* GetCharacterClassInfo() const { return CharacterClassInfo; }
	FORCEINLINE UAbilityInfo* GetAbilityInfo() const { return AbilityInfo; }

private:
	// 모든 직업 정보를 가진 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	// 플레이어의 모든 스킬 정보를 가진 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;
};
