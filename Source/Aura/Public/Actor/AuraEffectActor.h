// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "AuraEffectActor.generated.h"

class UGameplayEffect;

// 이펙트가 적용되는 규칙
UENUM(BlueprintType)
enum class EEffectApplicationPolicy
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

// 이펙트가 제거되는 규칙
UENUM(BlueprintType)
enum class EEffectRemovalPolicy
{
	RemoveOnEndOverlap,
	DoNotRemove
};

/**
 * 이펙트 효과를 가지고 있는 액터 클래스
 */
UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()
	
private:
	AAuraEffectActor();

private:
	virtual void BeginPlay() final;
	virtual void Tick(float DeltaTime) final;

private:
	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);

	// 블루프린트에서 콜리전들의 Begin Overlap, End Overlap에서 직접 연결할 함수들
	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);

	// 회전하기 시작 초기화
	UFUNCTION(BlueprintCallable)
	void StartRotation();

	// 위아래로 움직이기 시작 초기화
	UFUNCTION(BlueprintCallable)
	void StartSinusoidalMovement();

	// 드랍 아이템의 움직임 구현
	void ItemMovement(float DeltaTime);

protected:
	// 이펙트 적용 후 액터를 제거할 것인지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	bool bDestroyOnEffectApplication;

	// 적에게도 이펙트를 적용할 것인지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	bool bApplyEffectsToEnemies;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	EEffectApplicationPolicy InstantEffectApplicationPolicy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	EEffectApplicationPolicy DurationEffectApplicationPolicy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	EEffectApplicationPolicy InfiniteEffectApplicationPolicy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	EEffectRemovalPolicy InfiniteEffectRemovalPolicy;

	// 아이템 레벨
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	float ActorLevel;

	// 회전 효과를 킬 것인가
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement")
	bool bRotates = false;
	// 회전 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement")
	float RotationRate = 45.f;
	// 위아래로 움직일 것인가
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement")
	bool bSinusoidalMovement = false;
	// 위아래 움직이는 정도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement")
	float SineAmplitude = 8.f;
	// 주기
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement")
	float SinePeriodConstant = 4.f;
	// 움직임을 사용할 때 설정해놓을 초기 벡터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement")
	FVector InitialLocation;
	// 블루프린트에서 사용할 움직임 꼐산값
	UPROPERTY(BlueprintReadOnly)
	FVector CalculatedLocation;
	UPROPERTY(BlueprintReadOnly)
	FRotator CalculatedRotation;

private:
	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;

	// 위아래 움직일 때 사용할 시간값
	float RunningTime = 0.f;
};
