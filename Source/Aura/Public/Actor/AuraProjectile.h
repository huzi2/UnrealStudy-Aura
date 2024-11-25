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
 * 발사체 액터 클래스
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

	// 충돌했을 때 처리
	UFUNCTION(BlueprintCallable)
	virtual void OnHit();

	bool IsValidOverlap(AActor* OtherActor) const;

public:
	// 액터 스폰 전에 세팅하기 위해 이 변수를 노출
	// 데미지 정보를 가진 이펙트 파라미터 구조체
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	FDamageEffectParams DamageEffectParams;

protected:
	// 충돌 확인용 도형 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> Sphere;

	// 날아가는 동안 출력할 사운드 컴포넌트
	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;

	// 충돌했는 지 확인
	bool bHit = false;

private:
	// 생존 시간
	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 15.f;

	// 이동 컴포넌트
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	// 충돌 이펙트
	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;
	// 충돌 사운드
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;

	// 날아가는 동안 출력할 사운드
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopingSound;

	// 발사체가 타겟으로 잡은 컴포넌트
	UPROPERTY()
	TObjectPtr<USceneComponent> HomingTargetSceneComponent;
};
