// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraAbilityTypes.h"
#include "AuraProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;

/**
 * �߻�ü ���� Ŭ����
 */
UCLASS()
class AURA_API AAuraProjectile : public AActor
{
	GENERATED_BODY()
	
protected:
	AAuraProjectile();

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

public:
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
	USceneComponent* GetHomingTargetSceneComponent() const { return HomingTargetSceneComponent; }
	void SetHomingTargetSceneComponent(USceneComponent* InHomingTargetSceneComponent) { HomingTargetSceneComponent = InHomingTargetSceneComponent; }

protected:
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// �浹���� �� ó��
	UFUNCTION(BlueprintCallable)
	virtual void OnHit();

	bool IsValidOverlap(AActor* OtherActor) const;

public:
	// ���� ���� ���� �����ϱ� ���� �� ������ ����
	// ������ ������ ���� ����Ʈ �Ķ���� ����ü
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	FDamageEffectParams DamageEffectParams;

protected:
	// �浹 Ȯ�ο� ���� ������Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> Sphere;

	// ���ư��� ���� ����� ���� ������Ʈ
	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;

	// �浹�ߴ� �� Ȯ��
	bool bHit = false;

private:
	// ���� �ð�
	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 15.f;

	// �̵� ������Ʈ
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	// �浹 ����Ʈ
	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;
	// �浹 ����
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;

	// ���ư��� ���� ����� ����
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopingSound;

	// �߻�ü�� Ÿ������ ���� ������Ʈ
	UPROPERTY()
	TObjectPtr<USceneComponent> HomingTargetSceneComponent;
};
