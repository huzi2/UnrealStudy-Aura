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
	// 플레이어는 PlayerState에서 어빌리티와 어트리뷰트를 가짐
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	// 레벨은 복잡한 어트리뷰트 기능이 필요없고, float와 어울리지 않아서 어트리뷰트로 설정하지 않고 직접 사용한다.
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
	int32 Level;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
};
