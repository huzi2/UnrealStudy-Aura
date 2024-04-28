// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AuraPlayerState.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;

// �÷��̾� ������Ʈ�� ������ ����Ǿ��� �� ȣ���� ��������Ʈ
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, int32);

/**
 * ������ ����ġ�� �����ϴ� �÷��̾� ������Ʈ Ŭ����
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

private:
	AAuraPlayerState();

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const final;

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const final;

public:
	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
	FORCEINLINE int32 GetXP() const { return XP; }
	FORCEINLINE UAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
	void SetLevel(int32 InLevel);
	void AddToLevel(int32 InLevel);

	void SetXP(int32 InXP);
	void AddToXP(int32 InXP);

public:
	// ������ ����Ǿ��� �� ȣ���� ��������Ʈ
	FOnPlayerStateChanged OnLevelChangedDelegate;
	// XP�� ����Ǿ��� �� ȣ���� ��������Ʈ
	FOnPlayerStateChanged OnXPChangedDelegate;

private:
	// ������ ������ ��Ʈ����Ʈ ����� �ʿ����, float�� ��︮�� �ʾƼ� ��Ʈ����Ʈ�� �������� �ʰ� ���� ����Ѵ�.
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
	int32 Level = 1;
	UFUNCTION()
	void OnRep_Level(int32 OldLevel);
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_XP)
	int32 XP = 1;
	UFUNCTION()
	void OnRep_XP(int32 OldXP);

	// �÷��̾�� PlayerState���� �����Ƽ�� ��Ʈ����Ʈ�� ����
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	// ���� ����
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
};
