// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()

private:
	AAuraCharacter();

private:
	virtual void PossessedBy(AController* NewController) final;
	virtual void OnRep_PlayerState() final;

	virtual void InitAbilityActorInfo() final;
	virtual void InitializeDefaultAttributes() const final;

	virtual int32 GetPlayerLevel() const final;

protected:
	// �ɷ�ġ �ʱ�ȭ�� ����� �����÷��� ����Ʈ��
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;
};
