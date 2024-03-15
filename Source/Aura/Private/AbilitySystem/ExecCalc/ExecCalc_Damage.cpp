// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AuraGameplayTags.h"

// ��Ʈ����Ʈ�� ĸ�ĸ� ���� ���� ����ü
struct AuraDamageStatics
{
public:
	// ĸ�� ����(FGameplayEffectAttributeCaptureDefinition)�� ����
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);

	AuraDamageStatics()
	{
		// ĸ�� ������ ����
		// ��Ʈ����Ʈ ��Ʈ���� Ÿ���� Armor�� ��������, �������� ���� �ʴ´�.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
	}
};

// ���� �Լ��� ĸ�� ������ ���´�.
static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	// ������ ��꿡 ���� ��Ʈ����Ʈ���� ĸ�ĸ�Ͽ� ����
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// �����÷��� ����Ʈ���� Executions�� �� Ŭ������ �߰��ߴٸ� ����Ʈ ����� �� �� �Լ��� ���´�.
	// OutExecutionOutput�� ������ ��Ʈ����Ʈ���� �����ϸ� ����ȴ�.
	const UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();
	if (!SourceAbilitySystemComponent) return;

	const UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	if (!TargetAbilitySystemComponent) return;

	const AActor* SourceAvatar = SourceAbilitySystemComponent->GetAvatarActor();
	if (!SourceAvatar) return;

	const AActor* TargetAvatar = TargetAbilitySystemComponent->GetAvatarActor();
	if (!TargetAvatar) return;

	// ����Ʈ�� ��ü�� �±׿� Ÿ�ٿ� ���� �±׸� �����´�.
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// ��Ʈ����Ʈ�� ����� �� ���Ǵ� ����ü. �±׿� ���� ������ �޶����ų� �� �� ���
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// Caller Magnitude�� ������ �±׿� ����� ������ ���� ���´�.
	float Damage = Spec.GetSetByCallerMagnitude(UAuraGameplayTags::Get().Damage);

	// Ÿ���� BlockChance�� ����� Ȯ���� ����
	// ��Ͽ� �����ϸ� �������� �������� �پ���.

	// ĸ�ĵ� ���� ������ ����
	float TargetBlockChance = 0.f;
	// ���� ĸ��
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	// �������� ������ �ʵ��� Clamp
	TargetBlockChance = FMath::Max<float>(0.f, TargetBlockChance);

	if (FMath::FRandRange(1.f, 100.f) < TargetBlockChance) Damage *= 0.5f;

	// ���� ���� ���������� IncomingDamage ��Ʈ����Ʈ�� Additive
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
