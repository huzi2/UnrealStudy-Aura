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
	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;
	// 데미지를 입히는 이펙트 클래스
	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;
	// 데미지를 입히는 주체
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent = nullptr;
	// 데미지를 입는 주체
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent = nullptr;

	// 데미지량
	UPROPERTY()
	float BaseDamage = 0.f;
	// 스킬 레벨
	UPROPERTY()
	float AbilityLevel = 1.f;
	// 데미지 타입
	UPROPERTY()
	FGameplayTag DamageTypeTag = FGameplayTag();

	// 디버프 데미지 확률
	UPROPERTY()
	float DebuffChance = 0.f;
	// 디버프 데미지량
	UPROPERTY()
	float DebuffDamage = 0.f;
	// 디버프 데미지 지속시간
	UPROPERTY()
	float DebuffDuration = 0.f;
	// 디버프 데미지 주기
	UPROPERTY()
	float DebuffFrequency = 0.f;
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
	FORCEINLINE void SetIsBlockedHit(bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
	FORCEINLINE void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }

private:
	// 공격을 막았음
	UPROPERTY()
	bool bIsBlockedHit = false;
	// 공격에 치명타가 터짐
	UPROPERTY()
	bool bIsCriticalHit = false;
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
