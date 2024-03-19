// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"

// ��Ʈ����Ʈ�� ĸ�ĸ� ���� ���� ����ü
struct AuraDamageStatics
{
public:
	// ĸ�� ����(FGameplayEffectAttributeCaptureDefinition)�� ����
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);

	AuraDamageStatics()
	{
		// ĸ�� ������ ����
		// ��Ʈ����Ʈ ��Ʈ���� Ÿ���� Armor�� ��������, �������� ���� �ʴ´�.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);

		// �ڽ��� ������ �ִ� ��Ʈ����Ʈ
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
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
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
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

	// ������ ���� �Ĺ��������̽�
	const ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	if (!SourceCombatInterface) return;

	const AActor* TargetAvatar = TargetAbilitySystemComponent->GetAvatarActor();
	if (!TargetAvatar) return;

	const ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);
	if (!TargetCombatInterface) return;

	// ����� ����� Ŀ�� ���̺�
	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	if (!CharacterClassInfo) return;
	if (!CharacterClassInfo->DamageCalculationCoefficients) return;

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
	// ��ϵǸ� �������� ��������
	const bool bBlocked = FMath::FRandRange(1.f, 100.f) < TargetBlockChance;
	if (bBlocked) Damage *= 0.5f;

	// ������ ���� ��ġ��ŭ �ۼ�Ʈ�� �������� �氨��Ų��.
	// �尩������� Ÿ���� ������ �ۼ�Ʈ��ŭ �����Ѵ�.

	// Ÿ���� ����
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
	TargetArmor = FMath::Max<float>(0.f, TargetArmor);

	// ���� �尩�����
	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(0.f, SourceArmorPenetration);

	// �ڽ� ������ ���� �尩����� ����� ���̺��� ���´�.
	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(TEXT("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetPlayerLevel());

	// �尩������� ����� Ÿ���� ����
	const float EffectiveArmor = TargetArmor * (100.f - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;

	// Ÿ�� ������ ���� ��ȿ�� ���� ����� ���̺��� ���´�.
	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(TEXT("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel());

	// ������ �������� �������� �ۼ�Ʈ�� �氨
	Damage *= (100.f - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;

	// ġ��Ÿ Ȯ���� ���� ġ��Ÿ�� ������.
	// ġ��Ÿ Ȯ���� ����� ġ��Ÿ ���׷����� �����Ѵ�.
	// ġ��Ÿ �������� 2�� + ���� ġ��Ÿ ������

	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max<float>(0.f, SourceCriticalHitChance);

	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max<float>(0.f, TargetCriticalHitResistance);

	// Ÿ�� ������ ���� ġ��Ÿ ���׷� ����� ���̺��� ���´�.
	const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(TEXT("CriticalHitResistance"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetCombatInterface->GetPlayerLevel());

	// ���� ġ��Ÿ Ȯ���� ���� ġ��Ÿ Ȯ������ Ÿ���� ġ��Ÿ ���׷��� �� ��
	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;

	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max<float>(0.f, SourceCriticalHitDamage);
	
	// ġ��Ÿ�� �����ٸ� 2�� �������� ���� ġ��Ÿ ��������ŭ �����ش�.
	const bool bCriticalHit = FMath::FRandRange(1.f, 100.f) < EffectiveCriticalHitChance;
	if (bCriticalHit) Damage = Damage * 2.f + SourceCriticalHitDamage;

	// Ŀ���� �����÷��� ����Ʈ ���ؽ�Ʈ�� �� ����. �ٸ� ������ Ȯ���ϱ� ����
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);
	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);

	// ���� ���� ���������� IncomingDamage ��Ʈ����Ʈ�� Additive
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
