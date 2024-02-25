// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/ModMagCalc/MMC_MaxHealth.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	// 참고할 어트리뷰트 세트를 가져옴
	VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();
	// 타겟인지 소스인지 구분
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	// 스냅샷 설정
	VigorDef.bSnapshot = false;

	// 계산에 필요한 속성을 추가
	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// 이펙트에 자체의 태그와 타겟에 대한 태그를 가져온다.
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// 어트리뷰트를 계산할 때 사용되는 구조체. 태그에 따라 조건이 달라지거나 할 때 사용
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// Vigor에 저장된 값 얻어오기
	float Vigor = 0.f;
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluationParameters, Vigor);
	Vigor = FMath::Max<float>(Vigor, 0.f);

	// 소스 캐릭터에서 레벨 얻어오기
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	if (!CombatInterface) return 0.f;

	const int32 PlayerLevel = CombatInterface->GetPlayerLevel();

	// 최종 결과값. 기본값 80 + 활력 * 2.5 + 레벨 * 10
	return 80.f + (2.5f * Vigor) + (10.f * PlayerLevel);
}
