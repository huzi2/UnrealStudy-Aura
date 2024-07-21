// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"

// 어트리뷰트의 캡쳐를 위한 정적 구조체
struct AuraDamageStatics
{
public:
	// 캡쳐 변수(FGameplayEffectAttributeCaptureDefinition)를 선언
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
		// 캡쳐 변수를 정의
		// 어트리뷰트 세트에서 타겟의 Armor를 가져오고, 스냅샷은 하지 않는다.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);

		// 자신이 가지고 있는 어트리뷰트
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
	}
};

// 정적 함수로 캡쳐 변수를 얻어온다.
static AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics{};
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	// 데미지 계산에 사용될 어트리뷰트들을 캡쳐목록에 저장
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
	// 태그와 캡쳐를 서로 연결
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

	// 인터페이스에서 레벨을 얻어온다.
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

	// 계수에 사용할 커브 테이블
	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	if (!CharacterClassInfo) return;
	if (!CharacterClassInfo->DamageCalculationCoefficients) return;

	// 이펙트에 자체의 태그와 타겟에 대한 태그를 가져온다.
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// 어트리뷰트를 계산할 때 사용되는 구조체. 태그에 따라 조건이 달라지거나 할 때 사용
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	
	// 디버프 처리
	DetermineDebuff(ExecutionParams, Spec, EvaluationParameters, TagToCaptureDefs);

	const UAuraGameplayTags& GameplayTags = UAuraGameplayTags::Get();

	// Caller Magnitude에 설정한 태그에 저장된 데미지 값을 얻어온다.
	float Damage = 0.f;
	for (const TTuple<FGameplayTag, FGameplayTag>& Pair : GameplayTags.DamageTypesToResistances)
	{
		const FGameplayTag& DamageTypeTag = Pair.Key;
		const FGameplayTag& ResistanceTag = Pair.Value;

		float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag, false);
		if (DamageTypeValue == 0.f) continue;

		// 해당 공격 타입에 대한 저항값이 있다면 저항에 따라 데미지를 감소
		if (TagToCaptureDefs.Contains(ResistanceTag))
		{
			const FGameplayEffectAttributeCaptureDefinition& CaptureDef = TagToCaptureDefs[ResistanceTag];
			
			float ResistanceValue = 0.f;
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, ResistanceValue);
			// 저항값은 최대가 100이 되도록함
			ResistanceValue = FMath::Clamp(ResistanceValue, 0.f, 100.f);
			ResistanceValue = 50.f;

			DamageTypeValue *= (100.f - ResistanceValue) / 100.f;
		}

		Damage += DamageTypeValue;
	}

	// 타겟의 BlockChance로 블록할 확률을 구함
	// 블록에 성공하면 데미지가 절반으로 줄어든다.

	// 캡쳐된 값을 저장할 변수
	float TargetBlockChance = 0.f;
	// 값을 캡쳐
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	// 음수값이 나오지 않도록 Clamp
	TargetBlockChance = FMath::Max<float>(0.f, TargetBlockChance);
	// 블록되면 데미지를 절반으로
	//const bool bBlocked = FMath::FRandRange(1.f, 100.f) < TargetBlockChance;
	const bool bBlocked = FMath::FRandRange(1.f, 100.f) < 50.f;
	if (bBlocked) Damage *= 0.5f;

	// 방어력은 방어력 수치만큼 퍼센트로 데미지를 경감시킨다.
	// 장갑관통력이 타겟의 방어력의 퍼센트만큼 무시한다.

	// 타겟의 방어력
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
	TargetArmor = FMath::Max<float>(0.f, TargetArmor);

	// 나의 장갑관통력
	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(0.f, SourceArmorPenetration);

	// 자신 레벨에 따른 장갑관통력 계수를 테이블에서 얻어온다.
	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(TEXT("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourcePlayerLevel);

	// 장갑관통력을 계산한 타겟의 방어력
	const float EffectiveArmor = TargetArmor * (100.f - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;

	// 타겟 레벨에 따른 유효한 방어력 계수를 테이블에서 얻어온다.
	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(TEXT("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetPlayerLevel);

	// 유요한 방어력으로 데미지를 퍼센트로 경감
	Damage *= (100.f - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;

	// 치명타 확률에 따라 치명타가 터진다.
	// 치명타 확률은 상대의 치명타 저항력으로 감소한다.
	// 치명타 데미지는 2배 + 나의 치명타 데미지

	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max<float>(0.f, SourceCriticalHitChance);

	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max<float>(0.f, TargetCriticalHitResistance);

	// 타겟 레벨에 따른 치명타 저항력 계수를 테이블에서 얻어온다.
	const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(TEXT("CriticalHitResistance"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetPlayerLevel);

	// 최종 치명타 확률은 나의 치명타 확률에서 타겟의 치명타 저항력을 뺀 것
	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;

	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max<float>(0.f, SourceCriticalHitDamage);
	
	// 치명타가 터졌다면 2배 데미지에 나의 치명타 데미지만큼 더해준다.
	const bool bCriticalHit = FMath::FRandRange(1.f, 100.f) < 50.f;
	//const bool bCriticalHit = FMath::FRandRange(1.f, 100.f) < EffectiveCriticalHitChance;
	if (bCriticalHit) Damage = Damage * 2.f + SourceCriticalHitDamage;

	// 커스텀 게임플레이 이펙트 컨텍스트에 값 설정. 다른 곳에서 확인하기 위함
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);
	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);

	// 최종 계산된 데미지값을 IncomingDamage 어트리뷰트에 Additive
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}

void UExecCalc_Damage::DetermineDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FGameplayEffectSpec& Spec, const FAggregatorEvaluateParameters& EvaluationParameters, const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& TagToCaptureDefs) const
{
	const UAuraGameplayTags& GameplayTags = UAuraGameplayTags::Get();

	// 디버프 처리
	for (const TTuple<FGameplayTag, FGameplayTag>& Pair : GameplayTags.DamageTypesToDebuffs)
	{
		const FGameplayTag& DamageTypeTag = Pair.Key;
		const FGameplayTag& DebuffTypeTag = Pair.Value;

		const float TypeDamage = Spec.GetSetByCallerMagnitude(DamageTypeTag, false, -1.f);
		// 해당 타입에 디버프가 있음
		if (TypeDamage > -1.f)
		{
			// 디버프에 걸릴 확률
			const float SourceDebuffChance = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Chance, false, -1.f);

			// 디버프 저항값(속성 저항값을 그대로 사용)
			const FGameplayTag& ResistanceTag = GameplayTags.DamageTypesToResistances[DamageTypeTag];
			const FGameplayEffectAttributeCaptureDefinition& CaptureDef = TagToCaptureDefs[ResistanceTag];

			float ResistanceValue = 0.f;
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, ResistanceValue);
			ResistanceValue = FMath::Max<float>(ResistanceValue, 0.f);
			const float EffectiveDebuffChance = SourceDebuffChance * (100.f - ResistanceValue) / 100.f;

			// 디버프 걸렸는 지 확인
			const bool bDebuff = FMath::FRandRange(1.f, 100.f) < EffectiveDebuffChance;
			if (bDebuff)
			{
				// 이펙트 컨텍스트에 디버프 관련값 설정
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
