// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Damage.generated.h"

/**
 * 복잡한 데미지 계산을 따로 처리하기 위한 실행계산 클래스
 */
UCLASS()
class AURA_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
private:
	UExecCalc_Damage();

private:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const final;

private:
	// 디버프 적용
	void DetermineDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FGameplayEffectSpec& Spec, const FAggregatorEvaluateParameters& EvaluationParameters, const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& TagToCaptureDefs) const;
};
