// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraBeamSpell.generated.h"

/**
 * 감전 스킬에서 사용할 어빌리티 클래스
 */
UCLASS()
class AURA_API UAuraBeamSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
	
public:
	// 마우스 타겟 대상 정보를 저장
	UFUNCTION(BlueprintCallable)
	void StoreMouseDataInfo(const FHitResult& HitResult);

	// 소유자 관련 변수들을 저장
	UFUNCTION(BlueprintCallable)
	void StoreOwnerVariables();

	// 첫번째 타겟 자동 조준
	UFUNCTION(BlueprintCallable)
	void TraceFirstTarget(const FVector& BeamTargetLocation);

	// 감전 공격이 이어질 추가 타겟들 설정
	UFUNCTION(BlueprintCallable)
	void StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets);

	UFUNCTION(BlueprintImplementableEvent)
	void PrimaryTargetDied(AActor* DeadActor);

	UFUNCTION(BlueprintImplementableEvent)
	void AdditionalTargetDied(AActor* DeadActor);

protected:
	// 최대 연결될 타겟 개수
	UPROPERTY(EditDefaultsOnly, Category = "Beam")
	int32 MaxNumShockTargets = 5;

	// 타겟이 없을 때 찍을 땅 위치
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	FVector MouseHitLocation;
	// 타겟이 있을 때 타겟 액터
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<AActor> MouseHitActor;

	// 임시 변수들
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<APlayerController> OwnerPlayerController;
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<ACharacter> OwnerCharcter;
};
