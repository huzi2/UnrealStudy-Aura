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
 * Ŀ���� �÷��̾� ��Ʈ�ѷ� Ŭ����
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
	// ������ ��Ʈ����Ʈ�� ��Ÿ ��Ʈ����Ʈ�ν� ���ø�����Ʈ�����ʴ� ���� ��Ʈ����Ʈ��. �׷��� �� Ŭ�󿡼� �����ϱ����� Client RPC�� �����.
	// Ŭ���̾�Ʈ���� ������ UI�� ǥ��
	UFUNCTION(Client, Reliable)
	void ClientShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit);

	// ������ ����
	UFUNCTION(BlueprintCallable)
	void ShowMagicCircle(UMaterialInstance* DecalMaterial = nullptr);
	// ������ ����
	UFUNCTION(BlueprintCallable)
	void HideMagicCircle();

private:
	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent();

	// Ű �Է� ����
	// �̵�
	void Move(const FInputActionValue& InputActionValue);
	// ����Ʈ
	FORCEINLINE void ShiftPressed() { bShiftKeyDown = true; }
	FORCEINLINE void ShiftReleased() { bShiftKeyDown = false; }
	
	// Ŀ�� �Ʒ� Ÿ�� Ȯ��
	void CursorTrace();

	// �ڵ� �̵�
	void AutoRun();

	// �Է� �׼ǰ� ��ǲ �±׸� ����
	// ��ǲ �±׸� ������ ��
	void AbilityInputTagPressed(FGameplayTag InputTag);
	// ��ǲ �±׸� ���� ��
	void AbilityInputTagReleased(FGameplayTag InputTag);
	// ��ǲ �±׸� ������ ��
	void AbilityInputTagHeld(FGameplayTag InputTag);

	// �������� ��ġ�� ƽ���� Ŀ�� ��ġ�� �̵�
	void UpdateMagicCircleLocation();

private:
	// �����Ƽ �ý��� ������Ʈ
	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	// �Է� ����
	// ���� ���ý�Ʈ
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;
	// �̵� �׼�
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	// ����Ʈ �׼�
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ShiftAction;
	// ��ǲ �׼ǰ� ��ǲ �±� ���� Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	// Ŀ�� ���� ����
	// ���� ���� ����
	IHighlightInterface* LastActor;
	// ���� ���� ����
	IHighlightInterface* ThisActor;
	// Ŀ���� ����Ʈ���̽� ���
	FHitResult CursorHit;

	// Ŭ������ �� ǥ���� ����Ʈ
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ClickNiagaraSystem;

	// ������ ǥ�� ������Ʈ Ŭ����
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;

	// �̵� ����
	// ���ö��� ���� ������Ʈ
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;
	// ��ǥ ������ �����ؼ� �ڵ� �̵��� ������ �Ÿ�
	UPROPERTY(EditAnywhere)
	double AutoRunAcceptanceRadius = 50.0;
	// Ŭ���� ��ǥ ����
	FVector CachedDestination = FVector::ZeroVector;
	// Ŭ���� ������ �ð�
	float FollowTime = 0.f;
	// Ŭ���� ª�� Ŭ������ ������ ���� �ð�
	float ShortPressThreshold = 0.5f;
	// �ڵ����� �̵� ���ΰ�?
	bool bAutoRunning = false;
	// ���𰡸� Ÿ�� ���ΰ�?
	bool bTargeting = false;
	// ����ƮŰ�� ���� �����ΰ�?
	bool bShiftKeyDown = false;

	// ������ ������ Ŭ����
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMagicCircle> MagicCircleClass;
	// ������ ������ ����
	UPROPERTY()
	TObjectPtr<AMagicCircle> MagicCircle;
};
