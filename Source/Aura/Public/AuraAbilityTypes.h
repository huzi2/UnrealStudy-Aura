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
	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;
	// �������� ������ ����Ʈ Ŭ����
	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;
	// �������� ������ ��ü
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent = nullptr;
	// �������� �Դ� ��ü
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent = nullptr;

	// ��������
	UPROPERTY()
	float BaseDamage = 0.f;
	// ��ų ����
	UPROPERTY()
	float AbilityLevel = 1.f;
	// ������ Ÿ��
	UPROPERTY()
	FGameplayTag DamageTypeTag = FGameplayTag();

	// ����� ������ Ȯ��
	UPROPERTY()
	float DebuffChance = 0.f;
	// ����� ��������
	UPROPERTY()
	float DebuffDamage = 0.f;
	// ����� ������ ���ӽð�
	UPROPERTY()
	float DebuffDuration = 0.f;
	// ����� ������ �ֱ�
	UPROPERTY()
	float DebuffFrequency = 0.f;
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
	FORCEINLINE void SetIsBlockedHit(bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
	FORCEINLINE void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }

private:
	// ������ ������
	UPROPERTY()
	bool bIsBlockedHit = false;
	// ���ݿ� ġ��Ÿ�� ����
	UPROPERTY()
	bool bIsCriticalHit = false;
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
