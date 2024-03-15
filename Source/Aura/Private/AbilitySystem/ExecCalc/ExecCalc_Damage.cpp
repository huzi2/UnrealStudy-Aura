// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AuraGameplayTags.h"

// 어트리뷰트의 캡쳐를 위한 정적 구조체
struct AuraDamageStatics
{
public:
	// 캡쳐 변수(FGameplayEffectAttributeCaptureDefinition)를 선언
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);

	AuraDamageStatics()
	{
		// 캡쳐 변수를 정의
		// 어트리뷰트 세트에서 타겟의 Armor를 가져오고, 스냅샷은 하지 않는다.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
	}
};

// 정적 함수로 캡쳐 변수를 얻어온다.
static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	// 데미지 계산에 사용될 어트리뷰트들을 캡쳐목록에 저장
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// 게임플레이 이펙트에서 Executions에 이 클래스를 추가했다면 이펙트 적용될 때 이 함수로 들어온다.
	// OutExecutionOutput에 수정할 어트리뷰트들을 저장하면 적용된다.
	const UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();
	if (!SourceAbilitySystemComponent) return;

	const UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	if (!TargetAbilitySystemComponent) return;

	const AActor* SourceAvatar = SourceAbilitySystemComponent->GetAvatarActor();
	if (!SourceAvatar) return;

	const AActor* TargetAvatar = TargetAbilitySystemComponent->GetAvatarActor();
	if (!TargetAvatar) return;

	// 이펙트에 자체의 태그와 타겟에 대한 태그를 가져온다.
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// 어트리뷰트를 계산할 때 사용되는 구조체. 태그에 따라 조건이 달라지거나 할 때 사용
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// Caller Magnitude에 설정한 태그에 저장된 데미지 값을 얻어온다.
	float Damage = Spec.GetSetByCallerMagnitude(UAuraGameplayTags::Get().Damage);

	// 타겟의 BlockChance로 블록할 확률을 구함
	// 블록에 성공하면 데미지가 절반으로 줄어든다.

	// 캡쳐된 값을 저장할 변수
	float TargetBlockChance = 0.f;
	// 값을 캡쳐
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	// 음수값이 나오지 않도록 Clamp
	TargetBlockChance = FMath::Max<float>(0.f, TargetBlockChance);

	if (FMath::FRandRange(1.f, 100.f) < TargetBlockChance) Damage *= 0.5f;

	// 최종 계산된 데미지값을 IncomingDamage 어트리뷰트에 Additive
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
