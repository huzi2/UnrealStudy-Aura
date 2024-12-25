// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "GameplayTagContainer.h"
#include "PassiveNiagaraComponent.generated.h"

class UAuraAbilitySystemComponent;

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
	// �ش� �нú� ��ų�� ������ ����Ʈ Ȱ��ȭ/��Ȱ��ȭ
	void OnPassiveAbilityActivate(const FGameplayTag& AbilityTag, bool bActivate);

	// �����Ǿ��ִ� �нú� ��ų�̾����� ����Ʈ Ȱ��ȭ
	void ActivateIfEquipped(UAuraAbilitySystemComponent* AuraAbilitySystemComponent);

private:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag PassiveSpellTag;
};
