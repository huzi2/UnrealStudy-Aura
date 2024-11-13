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
 * 캐릭터와 적 캐릭터 베이스 클래스
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

	// IAbilitySystemInterface에서 상속
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	// ICombatInterface에서 상속
	virtual void Die(const FVector& DeathImpulse) override;
	virtual FOnAbilitySystemComponentRegistered& GetOnAbilitySystemComponentRegisteredDelegate() override;
	virtual FOnDeath& GetOnDeathDelegate() override;
	virtual FOnDamageSignature& GetOnDamageSignatureDelegate() override;

	// 기절 태그가 변경되었을 때 호출할 콜백 함수
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

private:
	virtual void InitAbilityActorInfo();
	virtual void InitializeDefaultAttributes() const;

	// ICombatInterface에서 상속
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
	// 캐릭터에게 이펙트 적용
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;

	// 캐릭터에게 어빌리티 부여
	void AddCharacterAbilities();

	// 소멸 이펙트 재생(블루프린트에서 작업)
	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);
	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

private:
	// 소멸 이펙트 재생
	void Dissolve();

	// 캐릭터가 죽었을 때 서버와 클라 모두 수행할 함수
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath(const FVector& DeathImpulse);

protected:
	// 게임플레이 어빌리티 시스템의 핵심 컴포넌트
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	// 어빌리티 시스템 컴포넌트가 등록되었을 때 호출할 델리게이트
	FOnAbilitySystemComponentRegistered OnAbilitySystemComponentRegisteredDelegate;

	// 캐릭터가 가지고 있는 어트리뷰트 세트들(능력치)
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	// 캐릭터의 직업
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	// 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float BaseWalkSpeed = 600.f;

	// 불타는 상태인가
	UPROPERTY(ReplicatedUsing=OnRep_Burned, BlueprintReadOnly)
	bool bIsBurned = false;
	UFUNCTION()
	virtual void OnRep_Burned() {};
	// 기절 상태인가
	UPROPERTY(ReplicatedUsing=OnRep_Stunned, BlueprintReadOnly)
	bool bIsStunned = false;
	UFUNCTION()
	virtual void OnRep_Stunned() {};
	// 감전 공격 받는 상태인가
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsBeInShocked = false;

	// 무기 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	// 불탈 때 사용할 디버프 이펙트 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Debuff")
	TObjectPtr<UDebuffNiagaraComponent> BurnDebuffComponent;

	// 기절될 때 사용할 디버프 이펙트 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Debuff")
	TObjectPtr<UDebuffNiagaraComponent> StunDebuffComponent;

	// 이펙트
	// 소멸 이펙트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;
	// 피격시 피 튀기는 이펙트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UNiagaraSystem> BloodEffect;

	// 사운드
	// 죽음 사운드
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USoundBase> DeathSound;

	// 자신이 죽었을 때 호출할 델리게이트
	FOnDeath OnDeathDelegate;

private:
	// 게임 플레이 어빌리티
	// 게임 시작부터 주어지는 어빌리티(능력)
	UPROPERTY(EditAnywhere, Category = "Abilites")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilites;
	// 게임 시작부터 주어지는 패시브 어빌리티
	UPROPERTY(EditAnywhere, Category = "Abilites")
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbilites;

	// 공격 소켓들
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponTipSocketName;
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName LeftHandSocketName;
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName RightHandSocketName;
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName TailSocketName;

	// 애니메이션
	// 피격 애니메이션
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
	// 공격 애니메이션
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FTaggedMontage> AttackMontages;

	// 패시브 스킬 이펙트가 붙을 곳
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> EffectAttachComponent;

	// 패시브 스킬 이펙트
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> HaloOfProtectionNiagaraComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> LifeSiphonNiagaraComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> ManaSiphonNiagaraComponent;

	// 죽었는가
	bool bDead = false;

	// 소환수 숫자
	int32 MinionCount = 0;

	// 데미지를 받았을 때 사용할 델리게이트
	FOnDamageSignature OnDamageSignatureDelegate;
};
