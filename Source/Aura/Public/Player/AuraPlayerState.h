// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AuraPlayerState.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

private:
	AAuraPlayerState();

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const final;

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const final;

public:
	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
	FORCEINLINE UAttributeSet* GetAttributeSet() const { return AttributeSet; }

private:
	// �÷��̾�� PlayerState���� �����Ƽ�� ��Ʈ����Ʈ�� ����
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	// ������ ������ ��Ʈ����Ʈ ����� �ʿ����, float�� ��︮�� �ʾƼ� ��Ʈ����Ʈ�� �������� �ʰ� ���� ����Ѵ�.
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
	int32 Level;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
};
