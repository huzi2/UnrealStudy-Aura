// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "GameplayTagContainer.h"
#include "PassiveNiagaraComponent.generated.h"

/**
 * �нú� ��ų Ȱ��ȭ�� �� ������ ����Ʈ ������Ʈ
 */
UCLASS()
class AURA_API UPassiveNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()
	
private:
	UPassiveNiagaraComponent();

private:
	virtual void BeginPlay() final;

private:
	void OnPassiveAbilityActivate(const FGameplayTag& AbilityTag, bool bActivate);

private:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag PassiveSpellTag;
};
