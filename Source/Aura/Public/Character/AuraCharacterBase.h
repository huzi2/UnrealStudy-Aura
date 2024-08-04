// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;
class UGameplayAbility;
class UDebuffNiagaraComponent;

/**
 * ĳ���Ϳ� �� ĳ���� ���̽� Ŭ����
 */
UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

protected:
	AAuraCharacterBase();

public:
	// IAbilitySystemInterface���� ���
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	// ICombatInterface���� ���
	virtual void Die() override;
	virtual FOnAbilitySystemComponentRegistered GetOnAbilitySystemComponentRegisteredDelegate() const override;
	virtual FOnDeath GetOnDeathDelegate() const override;

private:
	virtual void InitAbilityActorInfo();
	virtual void InitializeDefaultAttributes() const;

	virtual UAnimMontage* GetHitReactMontage_Implementation() const override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) const override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() const override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() const override;
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) const override;
	virtual int32 GetMinionCount_Implementation() const override;
	virtual void IncrementMinionCount_Implementation(int32 Amount) override;
	virtual ECharacterClass GetCharacterClass_Implementation() const override;

public:
	FORCEINLINE UAttributeSet* GetAttributeSet() const { return AttributeSet; }

protected:
	// ĳ���Ϳ��� ����Ʈ ����
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;

	// ĳ���Ϳ��� �����Ƽ �ο�
	void AddCharacterAbilities();

	// �Ҹ� ����Ʈ ���(�������Ʈ���� �۾�)
	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);
	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

private:
	// �Ҹ� ����Ʈ ���
	void Dissolve();

	// ĳ���Ͱ� �׾��� �� ������ Ŭ�� ��� ������ �Լ�
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();

protected:
	// �����÷��� �����Ƽ �ý����� �ٽ� ������Ʈ
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	// �����Ƽ �ý��� ������Ʈ�� ��ϵǾ��� �� ȣ���� ��������Ʈ
	FOnAbilitySystemComponentRegistered OnAbilitySystemComponentRegisteredDelegate;

	// ĳ���Ͱ� ������ �ִ� ��Ʈ����Ʈ ��Ʈ��(�ɷ�ġ)
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	FOnDeath OnDeathDelegate;

	// ĳ������ ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	// ���� ������Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	// ��Ż �� ����� ����� ����Ʈ ������Ʈ
	UPROPERTY(VisibleAnywhere, Category = "Debuff")
	TObjectPtr<UDebuffNiagaraComponent> BurnDebuffComponent;

	// ����Ʈ
	// �Ҹ� ����Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;
	// �ǰݽ� �� Ƣ��� ����Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UNiagaraSystem> BloodEffect;

	// ����
	// ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USoundBase> DeathSound;

private:
	// ���� �÷��� �����Ƽ
	// ���� ���ۺ��� �־����� �����Ƽ(�ɷ�)
	UPROPERTY(EditAnywhere, Category = "Abilites")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilites;
	// ���� ���ۺ��� �־����� �нú� �����Ƽ
	UPROPERTY(EditAnywhere, Category = "Abilites")
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbilites;

	// ���� ���ϵ�
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponTipSocketName;
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName LeftHandSocketName;
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName RightHandSocketName;
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName TailSocketName;

	// �ִϸ��̼�
	// �ǰ� �ִϸ��̼�
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
	// ���� �ִϸ��̼�
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FTaggedMontage> AttackMontages;

	// �׾��°�
	bool bDead = false;

	// ��ȯ�� ����
	int32 MinionCount = 0;
};
