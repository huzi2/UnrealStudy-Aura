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
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);

	AuraDamageStatics()
	{
		// ĸ�� ������ ����
		// ��Ʈ����Ʈ ��Ʈ���� Ÿ���� Armor�� ��������, �������� ���� �ʴ´�.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);

		// �ڽ��� ������ �ִ� ��Ʈ����Ʈ
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
	}
};

// ���� �Լ��� ĸ�� ������ ���´�.
static AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics{};
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
	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// �±׿� ĸ�ĸ� ���� ����
	const UAuraGameplayTags& Tags = UAuraGameplayTags::Get();
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagToCaptureDefs;
	
	TagToCaptureDefs.Add(Tags.Attribute_Secondary_Armor, DamageStatics().ArmorDef);
	TagToCaptureDefs.Add(Tags.Attribute_Secondary_ArmorPenetration, DamageStatics().ArmorPenetrationDef);
	TagToCaptureDefs.Add(Tags.Attribute_Secondary_BlockChance, DamageStatics().BlockChanceDef);
	TagToCaptureDefs.Add(Tags.Attribute_Secondary_CriticalHitChance, DamageStatics().CriticalHitChanceDef);
	TagToCaptureDefs.Add(Tags.Attribute_Secondary_CriticalHitDamage, DamageStatics().CriticalHitDamageDef);
	TagToCaptureDefs.Add(Tags.Attribute_Secondary_CriticalHitResistance, DamageStatics().CriticalHitResistanceDef);
	TagToCaptureDefs.Add(Tags.Attribute_Resistance_Fire, DamageStatics().FireResistanceDef);
	TagToCaptureDefs.Add(Tags.Attribute_Resistance_Lightning, DamageStatics().LightningResistanceDef);
	TagToCaptureDefs.Add(Tags.Attribute_Resistance_Arcane, DamageStatics().ArcaneResistanceDef);
	TagToCaptureDefs.Add(Tags.Attribute_Resistance_Physical, DamageStatics().PhysicalResistanceDef);

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

	// �������̽����� ������ ���´�.
	int32 SourcePlayerLevel = 1;
	if (SourceAvatar->Implements<UCombatInterface>())
	{
		SourcePlayerLevel = ICombatInterface::Execute_GetPlayerLevel(SourceAvatar);
	}

	int32 TargetPlayerLevel = 1;
	if (TargetAvatar->Implements<UCombatInterface>())
	{
		TargetPlayerLevel = ICombatInterface::Execute_GetPlayerLevel(TargetAvatar);
	}

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
	
	// ����� ó��
	DetermineDebuff(ExecutionParams, Spec, EvaluationParameters, TagToCaptureDefs);

	const UAuraGameplayTags& GameplayTags = UAuraGameplayTags::Get();

	// Caller Magnitude�� ������ �±׿� ����� ������ ���� ���´�.
	float Damage = 0.f;
	for (const TTuple<FGameplayTag, FGameplayTag>& Pair : GameplayTags.DamageTypesToResistances)
	{
		const FGameplayTag& DamageTypeTag = Pair.Key;
		const FGameplayTag& ResistanceTag = Pair.Value;

		float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag, false);
		if (DamageTypeValue == 0.f) continue;

		// �ش� ���� Ÿ�Կ� ���� ���װ��� �ִٸ� ���׿� ���� �������� ����
		if (TagToCaptureDefs.Contains(ResistanceTag))
		{
			const FGameplayEffectAttributeCaptureDefinition& CaptureDef = TagToCaptureDefs[ResistanceTag];
			
			float ResistanceValue = 0.f;
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, ResistanceValue);
			// ���װ��� �ִ밡 100�� �ǵ�����
			ResistanceValue = FMath::Clamp(ResistanceValue, 0.f, 100.f);
			ResistanceValue = 50.f;

			DamageTypeValue *= (100.f - ResistanceValue) / 100.f;
		}

		Damage += DamageTypeValue;
	}

	// Ÿ���� BlockChance�� ����� Ȯ���� ����
	// ��Ͽ� �����ϸ� �������� �������� �پ���.

	// ĸ�ĵ� ���� ������ ����
	float TargetBlockChance = 0.f;
	// ���� ĸ��
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	// �������� ������ �ʵ��� Clamp
	TargetBlockChance = FMath::Max<float>(0.f, TargetBlockChance);
	// ��ϵǸ� �������� ��������
	//const bool bBlocked = FMath::FRandRange(1.f, 100.f) < TargetBlockChance;
	const bool bBlocked = FMath::FRandRange(1.f, 100.f) < 50.f;
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
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourcePlayerLevel);

	// �尩������� ����� Ÿ���� ����
	const float EffectiveArmor = TargetArmor * (100.f - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;

	// Ÿ�� ������ ���� ��ȿ�� ���� ����� ���̺��� ���´�.
	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(TEXT("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetPlayerLevel);

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
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetPlayerLevel);

	// ���� ġ��Ÿ Ȯ���� ���� ġ��Ÿ Ȯ������ Ÿ���� ġ��Ÿ ���׷��� �� ��
	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;

	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max<float>(0.f, SourceCriticalHitDamage);
	
	// ġ��Ÿ�� �����ٸ� 2�� �������� ���� ġ��Ÿ ��������ŭ �����ش�.
	const bool bCriticalHit = FMath::FRandRange(1.f, 100.f) < 50.f;
	//const bool bCriticalHit = FMath::FRandRange(1.f, 100.f) < EffectiveCriticalHitChance;
	if (bCriticalHit) Damage = Damage * 2.f + SourceCriticalHitDamage;

	// Ŀ���� �����÷��� ����Ʈ ���ؽ�Ʈ�� �� ����. �ٸ� ������ Ȯ���ϱ� ����
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);
	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);

	// ���� ���� ���������� IncomingDamage ��Ʈ����Ʈ�� Additive
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}

void UExecCalc_Damage::DetermineDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FGameplayEffectSpec& Spec, const FAggregatorEvaluateParameters& EvaluationParameters, const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& TagToCaptureDefs) const
{
	const UAuraGameplayTags& GameplayTags = UAuraGameplayTags::Get();

	// ����� ó��
	for (const TTuple<FGameplayTag, FGameplayTag>& Pair : GameplayTags.DamageTypesToDebuffs)
	{
		const FGameplayTag& DamageTypeTag = Pair.Key;
		const FGameplayTag& DebuffTypeTag = Pair.Value;

		const float TypeDamage = Spec.GetSetByCallerMagnitude(DamageTypeTag, false, -1.f);
		// �ش� Ÿ�Կ� ������� ����
		if (TypeDamage > -1.f)
		{
			// ������� �ɸ� Ȯ��
			const float SourceDebuffChance = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Chance, false, -1.f);

			// ����� ���װ�(�Ӽ� ���װ��� �״�� ���)
			const FGameplayTag& ResistanceTag = GameplayTags.DamageTypesToResistances[DamageTypeTag];
			const FGameplayEffectAttributeCaptureDefinition& CaptureDef = TagToCaptureDefs[ResistanceTag];

			float ResistanceValue = 0.f;
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, ResistanceValue);
			ResistanceValue = FMath::Max<float>(ResistanceValue, 0.f);
			const float EffectiveDebuffChance = SourceDebuffChance * (100.f - ResistanceValue) / 100.f;

			// ����� �ɷȴ� �� Ȯ��
			const bool bDebuff = FMath::FRandRange(1.f, 100.f) < EffectiveDebuffChance;
			if (bDebuff)
			{
				// ����Ʈ ���ؽ�Ʈ�� ����� ���ð� ����
				FGameplayEffectContextHandle ContextHandle = Spec.GetContext();

				const float DebuffDamage = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Damage, false, -1.f);
				const float DebuffDuration = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Duration, false, -1.f);
				const float DebuffFrequency = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Frequency, false, -1.f);

				UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(ContextHandle, true);
				UAuraAbilitySystemLibrary::SetDebuffDamage(ContextHandle, DebuffDamage);
				UAuraAbilitySystemLibrary::SetDebuffDuration(ContextHandle, DebuffDuration);
				UAuraAbilitySystemLibrary::SetDebuffFrequency(ContextHandle, DebuffFrequency);
				UAuraAbilitySystemLibrary::SetDamageType(ContextHandle, DamageTypeTag);
			}
		}
	}
}
