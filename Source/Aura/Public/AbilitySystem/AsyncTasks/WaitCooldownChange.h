// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GameplayTagContainer.h"
#include "ActiveGameplayEffectHandle.h"
#include "WaitCooldownChange.generated.h"

class UAbilitySystemComponent;
struct FGameplayEffectSpec;

// ��Ÿ���� ���۵ǰų� ���� �� ����� ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownChangeSignature, float, TimeRemaining);

/**
 * �񵿱������� ��Ÿ���� üũ�ϴ� �������Ʈ ��� Ŭ����
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = "AsyncTask")) // �ڱ��ڽ��� �����ϴ� ���� ����
class AURA_API UWaitCooldownChange : public UBlueprintAsyncActionBase
{
	// �ٸ� �۾��� �������� �ʵ��� UBlueprintAsyncActionBase�� ��ӹ޾� �񵿱������� �����ϴ� �½�ũ�� ����
	// �񵿱� �۾��� ��׶��� �����忡�� ����ȴ�.
	GENERATED_BODY()

protected:
	// �������Ʈ������ ����� �� ����
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UWaitCooldownChange* WaitForCooldownChange(UAbilitySystemComponent* InAbilitySystemComponent, const FGameplayTag& InCooldownTag);

	UFUNCTION(BlueprintCallable)
	void EndTask();

private:
	void OnActiveEffectAdded(UAbilitySystemComponent* TargetAbilitySystemComponent, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle);
	void CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount);

public:
	// �������Ʈ���� �ƿ�ǲ ������ ���� ��������Ʈ��
	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownStart;
	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownEnd;

private:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	FGameplayTag CooldownTag;
};
