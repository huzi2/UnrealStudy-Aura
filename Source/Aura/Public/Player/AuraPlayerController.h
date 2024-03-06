// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "AuraPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;
class UAuraInputConfig;
struct FGameplayTag;
class UAuraAbilitySystemComponent;
class USplineComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
	
private:
	AAuraPlayerController();

private:
	virtual void BeginPlay() final;
	virtual void SetupInputComponent() final;
	virtual void PlayerTick(float DeltaTime) final;

private:
	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent();

	void Move(const FInputActionValue& InputActionValue);
	void CursorTrace();
	void AutoRun();

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	// �̵� ���� ����
	// ��ǥ ������ �����ؼ� �ڵ� �̵��� ������ �Ÿ�
	UPROPERTY(EditAnywhere)
	double AutoRunAcceptanceRadius;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

private:
	IEnemyInterface* LastActor;
	IEnemyInterface* ThisActor;
	FHitResult CursorHit;

	// �̵� ���� ����
	// Ŭ���� ��ǥ ����
	FVector CachedDestination;
	// Ŭ���� ������ �ð�
	float FollowTime;
	// Ŭ���� ª�� Ŭ������ ������ ���� �ð�
	float ShortPressThreshold;
	// �ڵ����� �̵� ���ΰ�?
	bool bAutoRunning;
	// ���𰡸� Ÿ�� ���ΰ�?
	bool bTargeting;
};
