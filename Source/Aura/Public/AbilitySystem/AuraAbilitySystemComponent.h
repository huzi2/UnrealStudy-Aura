// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

// ����� ����Ʈ ��ü�� ���� �±׸� Ȯ���ؼ� �ش� �±׿� ���� ��������Ʈ
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer&)
// �����Ƽ�� ���� ������ ������ ��������Ʈ
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven);
// �����Ƽ�� �ʿ��� ������ �ֱ����� ��������������Ʈ�ѷ����� �����ϰ� ForEachAbility()���� ������ ��������Ʈ
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);
// �����Ƽ ���°� ����Ǿ��� �� ����� ��������Ʈ. �����Ƽ �±׿� ���� �±�, �����Ƽ ����
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged, const FGameplayTag&, const FGameplayTag&, int32);
// ��ų ������ �� ȣ���� ��������Ʈ. �����Ƽ �±�, ���� �±�, ��ǲ �±�, ���� ��ǲ �±�
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquipped, const FGameplayTag&, const FGameplayTag&, const FGameplayTag&, const FGameplayTag&);
// �нú� ��ų ��Ȱ��ȭ�� �� ����� ��������Ʈ. �����Ƽ �±�
DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbility, const FGameplayTag&);

/**
 * Ŀ���� �����Ƽ �ý��� ������Ʈ
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

private:
	virtual void OnRep_ActivateAbilities() final;

public:
	// �����Ƽ���� �����Ƽ �±� ������
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	// �����Ƽ���� ��ǲ �±� ������
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	// �����Ƽ���� �����Ƽ�� ���� �±� ������
	static FGameplayTag GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec);

public:
	// �����Ƽ �±׿��� ���� �����Ƽ ������
	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);
	// �����Ƽ �±׿��� �����Ƽ�� ���� �±� ������
	FGameplayTag GetStatusFromAbilityTag(const FGameplayTag& AbilityTag);
	// �����Ƽ �±׿��� ��ǲ �±� ������
	FGameplayTag GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag);

	// �����Ƽ �±׿��� �����Ƽ ���� ������
	bool GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription);

	// ����Ʈ ����� �� �Լ��� ���ε�
	void AbilityActorInfoSet();

	// ĳ���Ϳ� �����Ƽ �߰�
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilites);
	// ĳ���Ϳ� �нú� �����Ƽ �߰�
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilites);

	// �Է��� ������ �� ����� �±� �Լ� ȣ��
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	// �Է��� ������Ǿ��� �� ����� �±� �Լ� ȣ��
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	// �Է��� �������� �� ����� �±� �Լ� ȣ��
	void AbilityInputTagHeld(const FGameplayTag& InputTag);

	// ����� ��� �����Ƽ ����
	void ForEachAbility(const FForEachAbility& Delegate);

	// �ɷ�ġ ���
	void UpgradeAttribute(const FGameplayTag& AttributeTag);

	// �������� �� �����Ƽ ���� ����
	void UpdateAbilityStatuses(int32 Level);

	// �������� ��ų ����Ʈ ��� ��û
	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag);

	// �������� ��ų ���� ��û
	UFUNCTION(Server, Reliable)
	void ServerEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag);

private:
	// �������� �ɷ�ġ ��� ��û
	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);

	// OnGameplayEffectAppliedDelegateToSelf ��������Ʈ�� ���������� ȣ��ȴ�(��������). �׷��� �Ʒ� �Լ��� Client RPC�� ����� Ŭ���̾�Ʈ������ ȣ��ǵ��� �Ѵ�.
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

	// Ŭ���̾�Ʈ���� �����Ƽ ���°� ����Ǿ����� �˸�
	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel);

	// Ŭ���̾�Ʈ���� ��ų ���� ����
	UFUNCTION(Client, Reliable)
	void ClientEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& InputTag, const FGameplayTag& PrevInputTag);

	// ��ų ���� ����
	// �ش� ��ų�� ���Կ��� ����
	void ClearSlot(FGameplayAbilitySpec& AbilitySpec);
	// �ش� ��ǲ �±׸� ����ϴ� ��� ��ų�� ���Կ��� ����
	void ClearAbilitiesOfSlot(const FGameplayTag& InputTag);

	// �ش� ������ ����ִ� �� Ȯ��
	bool SlotIsEmpty(const FGameplayTag& InputTag);
	// �ش� �����Ƽ�� �ش� ���Կ� �ִ� �� Ȯ��
	bool AbilityHasSlot(const FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& InputTag) const;

	// �ش� ���Կ��� ��ų ������
	FGameplayAbilitySpec* GetSpecWithSlot(const FGameplayTag& InputTag);

	// �ش� ��ų�� �нú� ��ų�� �� Ȯ��
	bool IsPassiveAbility(const FGameplayAbilitySpec& AbilitySpec) const;

public:
	// ����Ʈ�� ����� �±׿� ������ ��������Ʈ
	FEffectAssetTags EffectAssetTagsDelegate;
	// �����Ƽ�� ����� �� ��������Ʈ
	FAbilitiesGiven AbilitiesGivenDelegate;
	// �����Ƽ ���°� ������ �� ����� ��������Ʈ
	FAbilityStatusChanged AbilityStatusChangedDelegate;
	// ��ų ������ �� ȣ���� ��������Ʈ
	FAbilityEquipped AbilityEquippedDelegate;
	// �нú� ��ų ��Ȱ��ȭ�� �� ����� ��������Ʈ
	FDeactivatePassiveAbility DeactivatePassiveAbilityDelegate;
	// ���� �����Ƽ���� ������״� �� Ȯ��
	bool bStartupAbilitiesGiven = false;
};
