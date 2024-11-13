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
class UPassiveNiagaraComponent;

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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// IAbilitySystemInterface���� ���
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	// ICombatInterface���� ���
	virtual void Die(const FVector& DeathImpulse) override;
	virtual FOnAbilitySystemComponentRegistered& GetOnAbilitySystemComponentRegisteredDelegate() override;
	virtual FOnDeath& GetOnDeathDelegate() override;
	virtual FOnDamageSignature& GetOnDamageSignatureDelegate() override;

	// ���� �±װ� ����Ǿ��� �� ȣ���� �ݹ� �Լ�
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

private:
	virtual void InitAbilityActorInfo();
	virtual void InitializeDefaultAttributes() const;

	// ICombatInterface���� ���
	virtual UAnimMontage* GetHitReactMontage_Implementation() const override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) const override;
	virtual USkeletalMeshComponent* GetWeapon_Implementation() const override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() const override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() const override;
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) const override;
	virtual int32 GetMinionCount_Implementation() const override;
	virtual void IncrementMinionCount_Implementation(int32 Amount) override;
	virtual ECharacterClass GetCharacterClass_Implementation() const override;
	virtual bool IsBeInShocked_Implementation() const override;
	virtual void SetIsBeInShocked_Implementation(bool bInShock) override;

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
	virtual void MulticastHandleDeath(const FVector& DeathImpulse);

protected:
	// �����÷��� �����Ƽ �ý����� �ٽ� ������Ʈ
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	// �����Ƽ �ý��� ������Ʈ�� ��ϵǾ��� �� ȣ���� ��������Ʈ
	FOnAbilitySystemComponentRegistered OnAbilitySystemComponentRegisteredDelegate;

	// ĳ���Ͱ� ������ �ִ� ��Ʈ����Ʈ ��Ʈ��(�ɷ�ġ)
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	// ĳ������ ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	// �̵� �ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float BaseWalkSpeed = 600.f;

	// ��Ÿ�� �����ΰ�
	UPROPERTY(ReplicatedUsing=OnRep_Burned, BlueprintReadOnly)
	bool bIsBurned = false;
	UFUNCTION()
	virtual void OnRep_Burned() {};
	// ���� �����ΰ�
	UPROPERTY(ReplicatedUsing=OnRep_Stunned, BlueprintReadOnly)
	bool bIsStunned = false;
	UFUNCTION()
	virtual void OnRep_Stunned() {};
	// ���� ���� �޴� �����ΰ�
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsBeInShocked = false;

	// ���� ������Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	// ��Ż �� ����� ����� ����Ʈ ������Ʈ
	UPROPERTY(VisibleAnywhere, Category = "Debuff")
	TObjectPtr<UDebuffNiagaraComponent> BurnDebuffComponent;

	// ������ �� ����� ����� ����Ʈ ������Ʈ
	UPROPERTY(VisibleAnywhere, Category = "Debuff")
	TObjectPtr<UDebuffNiagaraComponent> StunDebuffComponent;

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

	// �ڽ��� �׾��� �� ȣ���� ��������Ʈ
	FOnDeath OnDeathDelegate;

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

	// �нú� ��ų ����Ʈ�� ���� ��
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> EffectAttachComponent;

	// �нú� ��ų ����Ʈ
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> HaloOfProtectionNiagaraComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> LifeSiphonNiagaraComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> ManaSiphonNiagaraComponent;

	// �׾��°�
	bool bDead = false;

	// ��ȯ�� ����
	int32 MinionCount = 0;

	// �������� �޾��� �� ����� ��������Ʈ
	FOnDamageSignature OnDamageSignatureDelegate;
};
