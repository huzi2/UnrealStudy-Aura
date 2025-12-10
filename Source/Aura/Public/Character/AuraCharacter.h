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
 * 플레이어 캐릭터 클래스
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

	// AAuraCharacterBase에서 상속
	virtual void InitAbilityActorInfo() final;
	virtual void InitializeDefaultAttributes() const final;
	virtual void OnRep_Burned() final;
	virtual void OnRep_Stunned() final;

	// ICombatInterface에서 상속
	virtual int32 GetPlayerLevel_Implementation() const final;
	virtual void Die(const FVector& DeathImpulse) final;

	// IPlayerInterface에서 상속
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

	// 다른 클라들도 이펙트를 볼 수 있도록 멀티캐스트 함수로 선언
	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpParticles() const;

	// 세이브 데이터에 저장된 능력치 로드
	void LoadProgress();

protected:
	// 능력치 초기화에 사용할 게임플레이 이펙트들
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;

	// 레벨 업 이펙트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

private:
	// 카메라 관련
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> TopDownCameraComponent;

	// 죽음 관련
	// 죽은 뒤 레벨 로드까지 걸리는 시간
	UPROPERTY(EditDefaultsOnly)
	float DeathTime = 5.f;
	FTimerHandle DeathTimer;
};
