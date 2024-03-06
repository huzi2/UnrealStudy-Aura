// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

// ����� ����Ʈ ��ü�� ���� �±׸� Ȯ���ؼ� �ش� �±׿� ���� ��������Ʈ
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& AssetTags)

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

private:
	// OnGameplayEffectAppliedDelegateToSelf ��������Ʈ�� ���������� ȣ��ȴ�(��������). �׷��� �Ʒ� �Լ��� Client RPC�� ����� Ŭ���̾�Ʈ������ ȣ��ǵ��� �Ѵ�.
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

public:
	void AbilityActorInfoSet();
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilites);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);

public:
	FEffectAssetTags EffectAssetTags;
};
