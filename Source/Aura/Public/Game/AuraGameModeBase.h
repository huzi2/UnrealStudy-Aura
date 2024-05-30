// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class UCharacterClassInfo;
class UAbilityInfo;

/**
 * ���Ӹ�� Ŭ����
 */
UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	FORCEINLINE UCharacterClassInfo* GetCharacterClassInfo() const { return CharacterClassInfo; }
	FORCEINLINE UAbilityInfo* GetAbilityInfo() const { return AbilityInfo; }

private:
	// ��� ���� ������ ���� Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	// �÷��̾��� ��� ��ų ������ ���� Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;
};
