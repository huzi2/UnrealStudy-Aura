#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

class UGameplayEffect;

/**
 * 데미지 이펙트에서 사용할 구조체
 */
USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

public:
	// 월드 오브젝트
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UObject> WorldContextObject = nullptr;
	// 데미지를 입히는 이펙트 클래스
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;
	// 데미지를 입히는 주체
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent = nullptr;
	// 데미지를 입는 주체
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent = nullptr;

	// 데미지량
	UPROPERTY(BlueprintReadWrite)
	float BaseDamage = 0.f;
	// 스킬 레벨
	UPROPERTY(BlueprintReadWrite)
	float AbilityLevel = 1.f;
	// 데미지 타입
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag DamageTypeTag = FGameplayTag();

	// 디버프 데미지 확률
	UPROPERTY(BlueprintReadWrite)
	float DebuffChance = 0.f;
	// 디버프 데미지량
	UPROPERTY(BlueprintReadWrite)
	float DebuffDamage = 0.f;
	// 디버프 데미지 지속시간
	UPROPERTY(BlueprintReadWrite)
	float DebuffDuration = 0.f;
	// 디버프 데미지 주기
	UPROPERTY(BlueprintReadWrite)
	float DebuffFrequency = 0.f;

	// 죽었을 때 충격 정도
	UPROPERTY(BlueprintReadWrite)
	float DeathImpulseMagnitude = 1000.f;
	// 죽었을 때 가할 충격 벡터
	UPROPERTY(BlueprintReadWrite)
	FVector DeathImpulse = FVector::ZeroVector;

	// 넉백 확률
	UPROPERTY(BlueprintReadWrite)
	float KnockbackChance = 20.f;
	// 넉백 충격 정도
	UPROPERTY(BlueprintReadWrite)
	float KnockbackForceMagnitude = 500.f;
	// 넉백에 가할 충격 벡터
	UPROPERTY(BlueprintReadWrite)
	FVector KnockbackForce = FVector::ZeroVector;

	// 방사형 데미지 관련
	// 방사형 데미지인가?
	UPROPERTY(BlueprintReadWrite)
	bool bIsRadialDamage = false;
	// 방사형 데미지 내각 크기
	UPROPERTY(BlueprintReadWrite)
	float RadialDamageInnerRadius = 0.f;
	// 방사형 데미지 외곽 크기
	UPROPERTY(BlueprintReadWrite)
	float RadialDamageOuterRadius = 0.f;
	// 방사형 데미지 원점
	UPROPERTY(BlueprintReadWrite)
	FVector RadialDamageOrigin = FVector::ZeroVector;
};

/**
 * 게임 플레이 이펙트 콘텍스트에 방어 유무와 치명타 유뮤를 추가한 구조체
 */
USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:
	// 구조체 반환
	virtual UScriptStruct* GetScriptStruct() const final
	{
		return StaticStruct();
	}

	// 직렬화
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) final;

	// 깊은 복사 수행
	virtual FAuraGameplayEffectContext* Duplicate() const final
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

public:
	FORCEINLINE bool IsBlockedHit() const { return bIsBlockedHit; }
	FORCEINLINE bool IsCriticalHit() const { return bIsCriticalHit; }
	FORCEINLINE bool IsSuccessfulDebuff() const { return bIsSuccessfulDebuff; }
	FORCEINLINE float GetDebuffDamage() const { return DebuffDamage; }
	FORCEINLINE float GetDebuffDuration() const { return DebuffDuration; }
	FORCEINLINE float GetDebuffFrequency() const { return DebuffFrequency; }
	FORCEINLINE TSharedPtr<FGameplayTag> GetDamageType() const { return DamageType; }
	FORCEINLINE FVector GetDeathImpulse() const { return DeathImpulse; }
	FORCEINLINE FVector GetKnockbackForce() const { return KnockbackForce; }
	FORCEINLINE bool IsRadialDamage() const { return bIsRadialDamage; }
	FORCEINLINE float GetRadialDamageInnerRadius() const { return RadialDamageInnerRadius; }
	FORCEINLINE float GetRadialDamageOuterRadius() const { return RadialDamageOuterRadius; }
	FORCEINLINE FVector GetRadialDamageOrigin() const { return RadialDamageOrigin; }

	FORCEINLINE void SetIsBlockedHit(bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
	FORCEINLINE void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
	FORCEINLINE void SetIsSuccessfulDebuff(bool bIsInSuccessfulDebuff) { bIsSuccessfulDebuff = bIsInSuccessfulDebuff; }
	FORCEINLINE void SetDebuffDamage(float InDebuffDamage) { DebuffDamage = InDebuffDamage; }
	FORCEINLINE void SetDebuffDuration(float InDebuffDuration) { DebuffDuration = InDebuffDuration; }
	FORCEINLINE void SetDebuffFrequency(float InDebuffFrequency) { DebuffFrequency = InDebuffFrequency; }
	FORCEINLINE void SetDamageType(TSharedPtr<FGameplayTag> InDamageType) { DamageType = InDamageType; }
	FORCEINLINE void SetDeathImpulse(const FVector& InDeathImpulse) { DeathImpulse = InDeathImpulse; }
	FORCEINLINE void SetKnockbackForce(const FVector& InKnockbackForce) { KnockbackForce = InKnockbackForce; }
	FORCEINLINE void SetIsRadialDamage(bool bInIsRadialDamage) { bIsRadialDamage = bInIsRadialDamage; }
	FORCEINLINE void SetRadialDamageInnerRadius(float InRadialDamageInnerRadius) { RadialDamageInnerRadius = InRadialDamageInnerRadius; }
	FORCEINLINE void SetRadialDamageOuterRadius(float InRadialDamageOuterRadius) { RadialDamageOuterRadius = InRadialDamageOuterRadius; }
	FORCEINLINE void SetRadialDamageOrigin(const FVector& InRadialDamageOrigin) { RadialDamageOrigin = InRadialDamageOrigin; }

private:
	// 공격을 막았음
	UPROPERTY()
	bool bIsBlockedHit = false;
	// 공격에 치명타가 터짐
	UPROPERTY()
	bool bIsCriticalHit = false;

	// 디버프 관련
	// 디버프 성공 여부
	UPROPERTY()
	bool bIsSuccessfulDebuff = false;
	// 디버프 데미지
	UPROPERTY()
	float DebuffDamage = 0.f;
	// 디버프 시간
	UPROPERTY()
	float DebuffDuration = 0.f;
	// 디버프 간격
	UPROPERTY()
	float DebuffFrequency = 0.f;

	// 데미지 타입 태그
	TSharedPtr<FGameplayTag> DamageType;

	// 죽었을 때 가할 충격 벡터
	UPROPERTY()
	FVector DeathImpulse = FVector::ZeroVector;

	// 넉백에 가할 충격 벡터
	UPROPERTY()
	FVector KnockbackForce = FVector::ZeroVector;

	// 방사형 데미지 관련
	// 방사형 데미지인가?
	UPROPERTY()
	bool bIsRadialDamage = false;
	// 방사형 데미지 내각 크기
	UPROPERTY()
	float RadialDamageInnerRadius = 0.f;
	// 방사형 데미지 외곽 크기
	UPROPERTY()
	float RadialDamageOuterRadius = 0.f;
	// 방사형 데미지 원점
	UPROPERTY()
	FVector RadialDamageOrigin = FVector::ZeroVector;
};

// 커스텀 NetSerialize()를 위해 위 구조체의 속성 정의
template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		// NetSerializer를 사용
		WithNetSerializer = true,
		// 구조체를 복사 가능
		WithCopy = true
	};
};
