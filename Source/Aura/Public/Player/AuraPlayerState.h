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

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

public:
	FORCEINLINE UAttributeSet* GetAttributeSet() const { return AttributeSet; }

private:
	// 플레이어는 PlayerState에서 어빌리티와 어트리뷰트를 가짐
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
};
