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
	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);

	// �������Ʈ���� �ݸ������� Begin Overlap, End Overlap���� ���� ������ �Լ���
	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	float ActorLevel;

private:
	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;
};
