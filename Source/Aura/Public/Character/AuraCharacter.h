// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

/**
 * �÷��̾� ĳ���� Ŭ����
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

	// AAuraCharacterBase���� ���
	virtual void InitAbilityActorInfo() final;
	virtual void InitializeDefaultAttributes() const final;

	// ICombatInterface���� ���
	virtual int32 GetPlayerLevel_Implementation() const final;

	// IPlayerInterface���� ���
	virtual void AddToXP_Implementation(int32 InXP) final;
	virtual void LevelUp_Implementation() final;

protected:
	// �ɷ�ġ �ʱ�ȭ�� ����� �����÷��� ����Ʈ��
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;
};
