// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

class UNiagaraComponent;
class UCameraComponent;
class USpringArmComponent;

/**
 * �÷��̾� ĳ���� Ŭ����
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()

private:
	AAuraCharacter();

private:
	virtual void PossessedBy(AController* NewController) final;
	virtual void OnRep_PlayerState() final;

	// AAuraCharacterBase���� ���
	virtual void InitAbilityActorInfo() final;
	virtual void InitializeDefaultAttributes() const final;
	virtual void OnRep_Burned() final;
	virtual void OnRep_Stunned() final;

	// ICombatInterface���� ���
	virtual int32 GetPlayerLevel_Implementation() const final;
	virtual void Die(const FVector& DeathImpulse) final;

	// IPlayerInterface���� ���
	virtual int32 FindLevelForXP_Implementation(int32 InXP) const final;
	virtual int32 GetXP_Implementation() const final;
	virtual void AddToXP_Implementation(int32 InXP) final;
	virtual void LevelUp_Implementation() final;
	virtual void AddToPlayerLevel_Implementation(int32 InPlayerLevel) final;
	virtual int32 GetAttributePoints_Implementation() const;
	virtual int32 GetAttributePointReward_Implementation(int32 Level) const final;
	virtual void AddToAttributePoints_Implementation(int32 InAttributePoints) final;
	virtual int32 GetSpellPoints_Implementation() const;
	virtual int32 GetSpellPointReward_Implementation(int32 Level) const final;
	virtual void AddToSpellPoints_Implementation(int32 InSpellPoints) final;
	virtual void ShowMagicCircle_Implementation(UMaterialInstance* DecalMaterial = nullptr) final;
	virtual void HideMagicCircle_Implementation() final;
	virtual void SaveProgress_Implementation(const FName& CheckpointTag) final;

	// �ٸ� Ŭ��鵵 ����Ʈ�� �� �� �ֵ��� ��Ƽĳ��Ʈ �Լ��� ����
	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpParticles() const;

	// ���̺� �����Ϳ� ����� �ɷ�ġ �ε�
	void LoadProgress();

protected:
	// �ɷ�ġ �ʱ�ȭ�� ����� �����÷��� ����Ʈ��
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;

	// ���� �� ����Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

private:
	// ī�޶� ����
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> TopDownCameraComponent;

	// ���� ����
	// ���� �� ���� �ε���� �ɸ��� �ð�
	UPROPERTY(EditDefaultsOnly)
	float DeathTime = 5.f;
	FTimerHandle DeathTimer;
};
