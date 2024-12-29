// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "AuraPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IHighlightInterface;
class UAuraInputConfig;
struct FGameplayTag;
class UAuraAbilitySystemComponent;
class USplineComponent;
class UDamageTextComponent;
class UNiagaraSystem;
class AMagicCircle;

/**
 * 커스텀 플레이어 컨트롤러 클래스
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

public:
	// 데미지 어트리뷰트가 메타 어트리뷰트로써 레플리케이트되지않는 서버 어트리뷰트다. 그래서 각 클라에서 실행하기위해 Client RPC로 만든다.
	// 클라이언트에게 데미지 UI를 표시
	UFUNCTION(Client, Reliable)
	void ClientShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit);

	// 마법진 생성
	UFUNCTION(BlueprintCallable)
	void ShowMagicCircle(UMaterialInstance* DecalMaterial = nullptr);
	// 마법진 제거
	UFUNCTION(BlueprintCallable)
	void HideMagicCircle();

private:
	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent();

	// 키 입력 관련
	// 이동
	void Move(const FInputActionValue& InputActionValue);
	// 쉬프트
	FORCEINLINE void ShiftPressed() { bShiftKeyDown = true; }
	FORCEINLINE void ShiftReleased() { bShiftKeyDown = false; }
	
	// 커서 아래 타겟 확인
	void CursorTrace();

	// 자동 이동
	void AutoRun();

	// 입력 액션과 인풋 태그를 연결
	// 인풋 태그를 눌렀을 때
	void AbilityInputTagPressed(FGameplayTag InputTag);
	// 인풋 태그를 뗐을 때
	void AbilityInputTagReleased(FGameplayTag InputTag);
	// 인풋 태그를 유지할 때
	void AbilityInputTagHeld(FGameplayTag InputTag);

	// 마법진의 위치를 틱마다 커서 위치로 이동
	void UpdateMagicCircleLocation();

private:
	// 어빌리티 시스템 컴포넌트
	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	// 입력 관련
	// 매핑 컨택스트
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;
	// 이동 액션
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	// 쉬프트 액션
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ShiftAction;
	// 인풋 액션과 인풋 태그 관리 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	// 커서 선택 관련
	// 이전 선택 액터
	IHighlightInterface* LastActor;
	// 현재 선택 액터
	IHighlightInterface* ThisActor;
	// 커서의 라인트레이스 결과
	FHitResult CursorHit;

	// 클릭했을 때 표시할 이펙트
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ClickNiagaraSystem;

	// 데미지 표시 컴포넌트 클래스
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;

	// 이동 관련
	// 스플라인 지원 컴포넌트
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;
	// 목표 지점에 도달해서 자동 이동을 중지할 거리
	UPROPERTY(EditAnywhere)
	double AutoRunAcceptanceRadius = 50.0;
	// 클릭한 목표 벡터
	FVector CachedDestination = FVector::ZeroVector;
	// 클릭을 유지한 시간
	float FollowTime = 0.f;
	// 클릭이 짧은 클릭으로 여겨질 기준 시간
	float ShortPressThreshold = 0.5f;
	// 자동으로 이동 중인가?
	bool bAutoRunning = false;
	// 무언가를 타겟 중인가?
	bool bTargeting = false;
	// 쉬프트키를 누른 상태인가?
	bool bShiftKeyDown = false;

	// 생성할 마법진 클래스
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMagicCircle> MagicCircleClass;
	// 생성된 마법진 액터
	UPROPERTY()
	TObjectPtr<AMagicCircle> MagicCircle;
};
