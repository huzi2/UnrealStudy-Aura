// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "AuraEffectActor.generated.h"

class UGameplayEffect;

// ����Ʈ�� ����Ǵ� ��Ģ
UENUM(BlueprintType)
enum class EEffectApplicationPolicy
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

// ����Ʈ�� ���ŵǴ� ��Ģ
UENUM(BlueprintType)
enum class EEffectRemovalPolicy
{
	RemoveOnEndOverlap,
	DoNotRemove
};

/**
 * ����Ʈ ȿ���� ������ �ִ� ���� Ŭ����
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

	// �������Ʈ���� �ݸ������� Begin Overlap, End Overlap���� ���� ������ �Լ���
	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);

	// ȸ���ϱ� ���� �ʱ�ȭ
	UFUNCTION(BlueprintCallable)
	void StartRotation();

	// ���Ʒ��� �����̱� ���� �ʱ�ȭ
	UFUNCTION(BlueprintCallable)
	void StartSinusoidalMovement();

	// ��� �������� ������ ����
	void ItemMovement(float DeltaTime);

protected:
	// ����Ʈ ���� �� ���͸� ������ ������
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	bool bDestroyOnEffectApplication;

	// �����Ե� ����Ʈ�� ������ ������
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

	// ������ ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	float ActorLevel;

	// ȸ�� ȿ���� ų ���ΰ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement")
	bool bRotates = false;
	// ȸ�� �ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement")
	float RotationRate = 45.f;
	// ���Ʒ��� ������ ���ΰ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement")
	bool bSinusoidalMovement = false;
	// ���Ʒ� �����̴� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement")
	float SineAmplitude = 8.f;
	// �ֱ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement")
	float SinePeriodConstant = 4.f;
	// �������� ����� �� �����س��� �ʱ� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement")
	FVector InitialLocation;
	// �������Ʈ���� ����� ������ ���갪
	UPROPERTY(BlueprintReadOnly)
	FVector CalculatedLocation;
	UPROPERTY(BlueprintReadOnly)
	FRotator CalculatedRotation;

private:
	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;

	// ���Ʒ� ������ �� ����� �ð���
	float RunningTime = 0.f;
};
