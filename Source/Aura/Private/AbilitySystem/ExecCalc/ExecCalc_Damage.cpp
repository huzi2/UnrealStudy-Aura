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

	AuraDamageStatics()
	{
		// 캡쳐 변수를 정의
		// 어트리뷰트 세트에서 타겟의 Armor를 가져오고, 스냅샷은 하지 않는다.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);

		// 자신이 가지고 있는 어트리뷰트
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
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
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
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

	// 레벨을 얻어올 컴뱃인터페이스
	const ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	if (!SourceCombatInterface) return;

	const AActor* TargetAvatar = TargetAbilitySystemComponent->GetAvatarActor();
	if (!TargetAvatar) return;

	const ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);
	if (!TargetCombatInterface) return;

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
	// 블록되면 데미지를 절반으로
	const bool bBlocked = FMath::FRandRange(1.f, 100.f) < TargetBlockChance;
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
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetPlayerLevel());

	// 장갑관통력을 계산한 타겟의 방어력
	const float EffectiveArmor = TargetArmor * (100.f - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;

	// 타겟 레벨에 따른 유효한 방어력 계수를 테이블에서 얻어온다.
	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(TEXT("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel());

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
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetCombatInterface->GetPlayerLevel());

	// 최종 치명타 확률은 나의 치명타 확률에서 타겟의 치명타 저항력을 뺀 것
	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;

	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max<float>(0.f, SourceCriticalHitDamage);
	
	// 치명타가 터졌다면 2배 데미지에 나의 치명타 데미지만큼 더해준다.
	const bool bCriticalHit = FMath::FRandRange(1.f, 100.f) < EffectiveCriticalHitChance;
	if (bCriticalHit) Damage = Damage * 2.f + SourceCriticalHitDamage;

	// 커스텀 게임플레이 이펙트 컨텍스트에 값 설정. 다른 곳에서 확인하기 위함
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);
	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);

	// 최종 계산된 데미지값을 IncomingDamage 어트리뷰트에 Additive
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
