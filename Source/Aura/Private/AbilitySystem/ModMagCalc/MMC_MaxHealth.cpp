// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/ModMagCalc/MMC_MaxHealth.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	// ������ ��Ʈ����Ʈ ��Ʈ�� ������
	VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();
	// Ÿ������ �ҽ����� ����
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	// ������ ����
	VigorDef.bSnapshot = false;

	// ��꿡 �ʿ��� �Ӽ��� �߰�
	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// ����Ʈ�� ��ü�� �±׿� Ÿ�ٿ� ���� �±׸� �����´�.
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// ��Ʈ����Ʈ�� ����� �� ���Ǵ� ����ü. �±׿� ���� ������ �޶����ų� �� �� ���
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// Vigor�� ����� �� ������
	float Vigor = 0.f;
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluationParameters, Vigor);
	Vigor = FMath::Max<float>(Vigor, 0.f);

	// �������̽��� ���� ������ ���´�.
	int32 PlayerLevel = 1;
	if (Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}

	// ���� �����. �⺻�� 80 + Ȱ�� * 2.5 + ���� * 10
	return 80.f + (2.5f * Vigor) + (10.f * static_cast<float>(PlayerLevel));
}
