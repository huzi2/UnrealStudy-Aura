// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GameplayTagContainer.h"
#include "ActiveGameplayEffectHandle.h"
#include "WaitCooldownChange.generated.h"

class UAbilitySystemComponent;
struct FGameplayEffectSpec;

// 쿨타임이 시작되거나 끝날 때 사용할 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownChangeSignature, float, TimeRemaining);

/**
 * 비동기적으로 쿨타임을 체크하는 블루프린트 노드 클래스
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = "AsyncTask")) // 자기자신을 리턴하는 핀을 생성
class AURA_API UWaitCooldownChange : public UBlueprintAsyncActionBase
{
	// 다른 작업을 차단하지 않도록 UBlueprintAsyncActionBase를 상속받아 비동기적으로 수행하는 태스크를 만듦
	// 비동기 작업은 백그라운드 스레드에서 수행된다.
	GENERATED_BODY()

protected:
	// 블루프린트에서만 사용할 수 있음
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UWaitCooldownChange* WaitForCooldownChange(UAbilitySystemComponent* InAbilitySystemComponent, const FGameplayTag& InCooldownTag);

	UFUNCTION(BlueprintCallable)
	void EndTask();

private:
	void OnActiveEffectAdded(UAbilitySystemComponent* TargetAbilitySystemComponent, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle);
	void CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount);

public:
	// 블루프린트에서 아웃풋 핀으로 사용될 델리게이트들
	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownStart;
	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownEnd;

private:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	FGameplayTag CooldownTag;
};
