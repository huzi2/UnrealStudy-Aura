// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "GameplayTagContainer.h"
#include "PassiveNiagaraComponent.generated.h"

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
	void OnPassiveAbilityActivate(const FGameplayTag& AbilityTag, bool bActivate);

private:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag PassiveSpellTag;
};
