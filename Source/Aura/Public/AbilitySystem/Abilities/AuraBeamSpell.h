// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraBeamSpell.generated.h"

/**
 * ���� ��ų���� ����� �����Ƽ Ŭ����
 */
UCLASS()
class AURA_API UAuraBeamSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
	
public:
	// ���콺 Ÿ�� ��� ������ ����
	UFUNCTION(BlueprintCallable)
	void StoreMouseDataInfo(const FHitResult& HitResult);

	// ������ ���� �������� ����
	UFUNCTION(BlueprintCallable)
	void StoreOwnerVariables();

	// ù��° Ÿ�� �ڵ� ����
	UFUNCTION(BlueprintCallable)
	void TraceFirstTarget(const FVector& BeamTargetLocation);

	// ���� ������ �̾��� �߰� Ÿ�ٵ� ����
	UFUNCTION(BlueprintCallable)
	void StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets);

	UFUNCTION(BlueprintImplementableEvent)
	void PrimaryTargetDied(AActor* DeadActor);

	UFUNCTION(BlueprintImplementableEvent)
	void AdditionalTargetDied(AActor* DeadActor);

protected:
	// �ִ� ����� Ÿ�� ����
	UPROPERTY(EditDefaultsOnly, Category = "Beam")
	int32 MaxNumShockTargets = 5;

	// Ÿ���� ���� �� ���� �� ��ġ
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	FVector MouseHitLocation;
	// Ÿ���� ���� �� Ÿ�� ����
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<AActor> MouseHitActor;

	// �ӽ� ������
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<APlayerController> OwnerPlayerController;
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<ACharacter> OwnerCharcter;
};
