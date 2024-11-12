#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

class UGameplayEffect;

/**
 * ������ ����Ʈ���� ����� ����ü
 */
USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

public:
	// ���� ������Ʈ
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UObject> WorldContextObject = nullptr;
	// �������� ������ ����Ʈ Ŭ����
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;
	// �������� ������ ��ü
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent = nullptr;
	// �������� �Դ� ��ü
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent = nullptr;

	// ��������
	UPROPERTY(BlueprintReadWrite)
	float BaseDamage = 0.f;
	// ��ų ����
	UPROPERTY(BlueprintReadWrite)
	float AbilityLevel = 1.f;
	// ������ Ÿ��
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag DamageTypeTag = FGameplayTag();

	// ����� ������ Ȯ��
	UPROPERTY(BlueprintReadWrite)
	float DebuffChance = 0.f;
	// ����� ��������
	UPROPERTY(BlueprintReadWrite)
	float DebuffDamage = 0.f;
	// ����� ������ ���ӽð�
	UPROPERTY(BlueprintReadWrite)
	float DebuffDuration = 0.f;
	// ����� ������ �ֱ�
	UPROPERTY(BlueprintReadWrite)
	float DebuffFrequency = 0.f;

	// �׾��� �� ��� ����
	UPROPERTY(BlueprintReadWrite)
	float DeathImpulseMagnitude = 1000.f;
	// �׾��� �� ���� ��� ����
	UPROPERTY(BlueprintReadWrite)
	FVector DeathImpulse = FVector::ZeroVector;

	// �˹� Ȯ��
	UPROPERTY(BlueprintReadWrite)
	float KnockbackChance = 20.f;
	// �˹� ��� ����
	UPROPERTY(BlueprintReadWrite)
	float KnockbackForceMagnitude = 500.f;
	// �˹鿡 ���� ��� ����
	UPROPERTY(BlueprintReadWrite)
	FVector KnockbackForce = FVector::ZeroVector;

	// ����� ������ ����
	// ����� �������ΰ�?
	UPROPERTY(BlueprintReadWrite)
	bool bIsRadialDamage = false;
	// ����� ������ ���� ũ��
	UPROPERTY(BlueprintReadWrite)
	float RadialDamageInnerRadius = 0.f;
	// ����� ������ �ܰ� ũ��
	UPROPERTY(BlueprintReadWrite)
	float RadialDamageOuterRadius = 0.f;
	// ����� ������ ����
	UPROPERTY(BlueprintReadWrite)
	FVector RadialDamageOrigin = FVector::ZeroVector;
};

/**
 * ���� �÷��� ����Ʈ ���ؽ�Ʈ�� ��� ������ ġ��Ÿ ���¸� �߰��� ����ü
 */
USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:
	// ����ü ��ȯ
	virtual UScriptStruct* GetScriptStruct() const final
	{
		return StaticStruct();
	}

	// ����ȭ
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) final;

	// ���� ���� ����
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
	// ������ ������
	UPROPERTY()
	bool bIsBlockedHit = false;
	// ���ݿ� ġ��Ÿ�� ����
	UPROPERTY()
	bool bIsCriticalHit = false;

	// ����� ����
	// ����� ���� ����
	UPROPERTY()
	bool bIsSuccessfulDebuff = false;
	// ����� ������
	UPROPERTY()
	float DebuffDamage = 0.f;
	// ����� �ð�
	UPROPERTY()
	float DebuffDuration = 0.f;
	// ����� ����
	UPROPERTY()
	float DebuffFrequency = 0.f;

	// ������ Ÿ�� �±�
	TSharedPtr<FGameplayTag> DamageType;

	// �׾��� �� ���� ��� ����
	UPROPERTY()
	FVector DeathImpulse = FVector::ZeroVector;

	// �˹鿡 ���� ��� ����
	UPROPERTY()
	FVector KnockbackForce = FVector::ZeroVector;

	// ����� ������ ����
	// ����� �������ΰ�?
	UPROPERTY()
	bool bIsRadialDamage = false;
	// ����� ������ ���� ũ��
	UPROPERTY()
	float RadialDamageInnerRadius = 0.f;
	// ����� ������ �ܰ� ũ��
	UPROPERTY()
	float RadialDamageOuterRadius = 0.f;
	// ����� ������ ����
	UPROPERTY()
	FVector RadialDamageOrigin = FVector::ZeroVector;
};

// Ŀ���� NetSerialize()�� ���� �� ����ü�� �Ӽ� ����
template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		// NetSerializer�� ���
		WithNetSerializer = true,
		// ����ü�� ���� ����
		WithCopy = true
	};
};
