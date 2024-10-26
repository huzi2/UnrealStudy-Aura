// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "CombatInterface.generated.h"

class UNiagaraSystem;
class UAbilitySystemComponent;

// �����Ƽ �ý��� ������Ʈ�� ��ϵǾ��� �� ȣ���� ��������Ʈ
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAbilitySystemComponentRegistered, UAbilitySystemComponent*);
// ĳ���Ͱ� �׾��� �� ȣ���� ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, AActor*, DeadActor);

/**
 * �±׿� �ִ� ��Ÿ�ָ� ����
 */
USTRUCT(BlueprintType)
struct FTaggedMontage
{
	GENERATED_BODY()

public:
	// ��Ÿ�� ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* Montage = nullptr;

	// ���� ��Ÿ�� ������ �±�
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag MontageTag;

	// ���� ��Ÿ�ְ� ����� ������ �±�
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag SocketTag;

	// �ش� �������� ����� ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* ImpactSound = nullptr;
};

UINTERFACE(MinimalAPI, BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * ������ ���õ� ������ ��ų� �ൿ�� �ϴ� �������̽�
 */
class AURA_API ICombatInterface
{
	GENERATED_BODY()

public:
	virtual void Die(const FVector& DeathImpulse) = 0;
	virtual FOnAbilitySystemComponentRegistered& GetOnAbilitySystemComponentRegisteredDelegate() = 0;
	virtual FOnDeath& GetOnDeathDelegate() = 0;

protected:
	// ĳ������ ������ ���´�.
	UFUNCTION(BlueprintNativeEvent)
	int32 GetPlayerLevel() const;

	// ��ǿ��� ������Ʈ�� ���� ��ü�� Ÿ���� ���ϵ��� �Ѵ�.
	// �����Ƽ�� �������Ʈ�� ���Ϳ� ���ӵ����ʵ��� �������̽����� ����
	// ��ǿ��� ������Ʈ�� �ڵ尡 �ƴ϶� �������Ʈ���� ���� �߰������Ƿ� �Լ����뵵 �������Ʈ���� ����
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateFacingTarget(const FVector& Target);

	// ���� ��� ���� ���·� �����Ѵ�.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetInShockLoop(bool bInShockLoop);

	// �ǰ� ���� �׼��� ���´�.
	// BlueprintNativeEvent�� �������Ʈ������ ������ �����ϰ� C++���� ������ �����ϴ�. �׸��� �ڵ������� virtual �Ӽ��� �ٰԵ�
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetHitReactMontage() const;

	// ��� ���� ����� ���´�.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<FTaggedMontage> GetAttackMontages() const;

	// ���� ��ǿ� ���� ���� ������ ���´�.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetCombatSocketLocation(const FGameplayTag& MontageTag) const;

	// ������ �޽� ������Ʈ�� ���´�.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	USkeletalMeshComponent* GetWeapon() const;

	// �׾��� �� Ȯ��
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDead() const;

	// �ƹ�Ÿ ���͸� ���´�.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetAvatar();

	// �ǰ� ����Ʈ�� ���´�.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UNiagaraSystem* GetBloodEffect() const;

	// �±׿� ���� ��Ÿ�� ������ ���´�.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FTaggedMontage GetTaggedMontageByTag(const FGameplayTag& MontageTag) const;

	// ��ȯ�� ���� ���� ���´�.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetMinionCount() const;

	// ��ȯ�� ���� ���� ����
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IncrementMinionCount(int32 Amount);

	// ��ü�� ������ ���´�.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	ECharacterClass GetCharacterClass() const;

	// ���� ���� �޴� �����ΰ�
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsBeInShocked() const;
	// ���� ���� �޴� ���� ����
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetIsBeInShocked(bool bInShock);
};
