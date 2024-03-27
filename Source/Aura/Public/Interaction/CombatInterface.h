// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API ICombatInterface
{
	GENERATED_BODY()

public:
	virtual void Die() = 0;
	virtual int32 GetPlayerLevel() const;

protected:
	// �����Ƽ�� �������Ʈ�� ���Ϳ� ���ӵ����ʵ��� �������̽����� ����
	// ��ǿ��� ������Ʈ�� �ڵ尡 �ƴ϶� �������Ʈ���� ���� �߰������Ƿ� �Լ����뵵 �������Ʈ���� ����
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateFacingTarget(const FVector& Target);

	// BlueprintNativeEvent�� �������Ʈ������ ������ �����ϰ� C++���� ������ �����ϴ�. �׸��� �ڵ������� virtual �Ӽ��� �ٰԵ�
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetHitReactMontage() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetCombatSocketLocation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDead() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetAvatar();
};
