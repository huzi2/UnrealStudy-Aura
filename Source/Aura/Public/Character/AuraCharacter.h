// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

/**
 * 플레이어 캐릭터 클래스
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()

private:
	AAuraCharacter();

private:
	virtual void PossessedBy(AController* NewController) final;
	virtual void OnRep_PlayerState() final;

	// AAuraCharacterBase에서 상속
	virtual void InitAbilityActorInfo() final;
	virtual void InitializeDefaultAttributes() const final;

	// ICombatInterface에서 상속
	virtual int32 GetPlayerLevel_Implementation() const final;

	// IPlayerInterface에서 상속
	virtual void AddToXP_Implementation(int32 InXP) final;
	virtual void LevelUp_Implementation() final;

protected:
	// 능력치 초기화에 사용할 게임플레이 이펙트들
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;
};
