// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/AuraProjectile.h"
#include "AuraFireBall.generated.h"

/**
 * ȭ�� ���߿��� ����� �߻�ü Ŭ����
 */
UCLASS()
class AURA_API AAuraFireBall : public AAuraProjectile
{
	GENERATED_BODY()

private:
	virtual void BeginPlay() final;

	// AAuraProjectile���� ���
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) final;
	virtual void OnHit() final;

public:
	FORCEINLINE void SetReturnToActor(AActor* InReturnToActor) { ReturnToActor = InReturnToActor; }
	FORCEINLINE void SetExplosionDamageEffectParams(const FDamageEffectParams& InExplosionDamageEffectParams) { ExplosionDamageEffectParams = InExplosionDamageEffectParams; }

	// ȭ������ �߻�Ǿ��ٰ� ���ƿ��� Ÿ�Ӷ����� �����ϴ� �̺�Ʈ �Լ�
	UFUNCTION(BlueprintImplementableEvent)
	void StartOutgoingTimeline();

protected:
	// ȭ������ �ǵ��ƿ� Ÿ�� ����(������)
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> ReturnToActor;

	// ȭ������ ������ �� ����� ������ ����Ʈ �Ķ�
	UPROPERTY(BlueprintReadWrite)
	FDamageEffectParams ExplosionDamageEffectParams;
};