// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "GameplayTagContainer.h"
#include "PassiveNiagaraComponent.generated.h"

class UAuraAbilitySystemComponent;

/**
 * 패시브 스킬 활성화될 때 보여줄 이펙트 컴포넌트
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
	// 해당 패시브 스킬이 맞으면 이펙트 활성화/비활성화
	void OnPassiveAbilityActivate(const FGameplayTag& AbilityTag, bool bActivate);

	// 장착되어있던 패시브 스킬이었으면 이펙트 활성화
	void ActivateIfEquipped(UAuraAbilitySystemComponent* AuraAbilitySystemComponent);

private:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag PassiveSpellTag;
};
