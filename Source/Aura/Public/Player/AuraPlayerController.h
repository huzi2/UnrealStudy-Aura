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

	// 이동 관련 변수
	// 목표 지점에 도달해서 자동 이동을 중지할 거리
	UPROPERTY(EditAnywhere)
	double AutoRunAcceptanceRadius;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

private:
	IEnemyInterface* LastActor;
	IEnemyInterface* ThisActor;
	FHitResult CursorHit;

	// 이동 관련 변수
	// 클릭한 목표 벡터
	FVector CachedDestination;
	// 클릭을 유지한 시간
	float FollowTime;
	// 클릭이 짧은 클릭으로 여겨질 기준 시간
	float ShortPressThreshold;
	// 자동으로 이동 중인가?
	bool bAutoRunning;
	// 무언가를 타겟 중인가?
	bool bTargeting;
};
