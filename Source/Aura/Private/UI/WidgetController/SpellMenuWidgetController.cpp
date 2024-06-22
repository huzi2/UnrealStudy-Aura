// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	if (!GetAuraAbilitySystemComponent()) return;
	if (!GetAuraPlayerState()) return;

	// �����Ƽ ���°� ������ �� UI�� �˷��ִ� �Լ� ���ε�
	GetAuraAbilitySystemComponent()->AbilityStatusChangedDelegate.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
	{
		// ���� ���õ� ��ų�� ���°� ����Ǿ��ٸ�
		if (SelectedAbility.AbilityTag.MatchesTagExact(AbilityTag))
		{
			SelectedAbility.StatusTag = StatusTag;

			// ��ų ����Ʈ�� ���� ��ư�� Ȱ��ȭ ���� Ȯ��
			bool bShouldEnableSpellPointsButton = false;
			bool bShouldEnableEquipButton = false;

			ShouldEnableButton(StatusTag, CurrentSpellPoints, bShouldEnableSpellPointsButton, bShouldEnableEquipButton);

			FString Description = FString();
			FString NextLevelDescription = FString();
			GetAuraAbilitySystemComponent()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);

			// Ȱ��ȭ ������ ��ų ������ UI���� ��������Ʈ�� �˸�
			SpellGlobeSelectedDelegate.Broadcast(bShouldEnableSpellPointsButton, bShouldEnableEquipButton, Description, NextLevelDescription);
		}

		if (AbilityInfo)
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			// ���� �±� ����
			Info.StatusTag = StatusTag;
			// ��� UI���� �ش� �����Ƽ�� �����Ǿ����� �˸�
			AbilityInfoDelegate.Broadcast(Info);
		}
	});

	// �÷��̾� ������Ʈ���� ��ų ����Ʈ�� ��ȭ�� �� ����ϴ� ��������Ʈ�� �Լ� ���ε�
	GetAuraPlayerState()->OnSpellPointsChangedDelegate.AddLambda([this](int32 SpellPoints)
	{
		SpellPointsChangedDelegate.Broadcast(SpellPoints);
		CurrentSpellPoints = SpellPoints;

		// ��ų ����Ʈ�� ���� ��ư�� Ȱ��ȭ ���� Ȯ��
		bool bShouldEnableSpellPointsButton = false;
		bool bShouldEnableEquipButton = false;

		ShouldEnableButton(SelectedAbility.StatusTag, CurrentSpellPoints, bShouldEnableSpellPointsButton, bShouldEnableEquipButton);

		FString Description = FString();
		FString NextLevelDescription = FString();
		GetAuraAbilitySystemComponent()->GetDescriptionsByAbilityTag(SelectedAbility.AbilityTag, Description, NextLevelDescription);

		// Ȱ��ȭ ������ ��ų ������ UI���� ��������Ʈ�� �˸�
		SpellGlobeSelectedDelegate.Broadcast(bShouldEnableSpellPointsButton, bShouldEnableEquipButton, Description, NextLevelDescription);
	});
}

void USpellMenuWidgetController::BroadcastInitialValue()
{
	if (!GetAuraPlayerState()) return;

	// �⺻ �����Ƽ ������ ��ε�ĳ��Ʈ�ؼ� EquippedSpellRow�� UI�� ����
	BroadcastAbilityInfo();

	// ��ų ����Ʈ �ʱⰪ�� UI�� �˸�
	SpellPointsChangedDelegate.Broadcast(GetAuraPlayerState()->GetSpellPoints());
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	if (!GetAuraPlayerState()) return;
	if (!GetAuraAbilitySystemComponent()) return;

	// ��ų ������ ��ٸ��� ���¶��
	if (bWaitingForEquipSelection && AbilityInfo)
	{
		const FGameplayTag AbilityTypeTag = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.AbilityTag).AbilityTypeTag;
		// �ٸ� ��ų�� �����߰ų� �ٽ� �����ϸ� ��ų ���� ��ٸ��� ���µ� ���
		StopWaitingForEquipDelegate.Broadcast(AbilityTypeTag);
		bWaitingForEquipSelection = false;
	}

	const int32 SpellPoints = GetAuraPlayerState()->GetSpellPoints();

	const UAuraGameplayTags GameplayTags = UAuraGameplayTags::Get();
	const bool bTagValid = AbilityTag.IsValid();
	const bool bTagNone = AbilityTag.MatchesTag(GameplayTags.Abilities_None);
	const FGameplayAbilitySpec* AbilitySpec = GetAuraAbilitySystemComponent()->GetSpecFromAbilityTag(AbilityTag);
	const bool bSpecValid = AbilitySpec != nullptr;

	FGameplayTag StatusTag;
	// �����Ƽ�� ��ȿ���� ����
	if (!bTagValid || bTagNone || !bSpecValid)
	{
		StatusTag = GameplayTags.Abilities_Status_Locked;
	}
	// �����Ƽ�� ��ȿ��
	else
	{
		StatusTag = GetAuraAbilitySystemComponent()->GetStatusFromSpec(*AbilitySpec);
	}

	// ���� ���õǾ��ִ� ��ų�� ���°� ����Ǿ��� �� ��������Ʈ���� �����ϱ����� ������ ����
	SelectedAbility.AbilityTag = AbilityTag;
	SelectedAbility.StatusTag = StatusTag;
	CurrentSpellPoints = SpellPoints;

	// ��ų ����Ʈ�� ���� ��ư�� Ȱ��ȭ ���� Ȯ��
	bool bShouldEnableSpellPointsButton = false;
	bool bShouldEnableEquipButton = false;

	ShouldEnableButton(StatusTag, SpellPoints, bShouldEnableSpellPointsButton, bShouldEnableEquipButton);

	FString Description = FString();
	FString NextLevelDescription = FString();
	GetAuraAbilitySystemComponent()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);

	// ��ư Ȱ��ȭ ������ ��ų ������ UI���� ��������Ʈ�� �˸�
	SpellGlobeSelectedDelegate.Broadcast(bShouldEnableSpellPointsButton, bShouldEnableEquipButton, Description, NextLevelDescription);
}

void USpellMenuWidgetController::SpellGlobeDeselect()
{
	// ��ų ������ ��ٸ��� ���¶��
	if (bWaitingForEquipSelection && AbilityInfo)
	{
		const FGameplayTag AbilityTypeTag = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.AbilityTag).AbilityTypeTag;
		// ��ų ������ ��ҵǾ����� ��ų ���� ��ٸ��� ���µ� ���
		StopWaitingForEquipDelegate.Broadcast(AbilityTypeTag);
		bWaitingForEquipSelection = false;
	}

	SelectedAbility.AbilityTag = UAuraGameplayTags::Get().Abilities_None;
	SelectedAbility.StatusTag = UAuraGameplayTags::Get().Abilities_Status_Locked;

	// ��ư Ȱ��ȭ ������ ��ų ���� �ʱ�ȭ
	SpellGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());
}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
	if (!GetAuraAbilitySystemComponent()) return;

	// �������� �ش� ��ų���� ��ų ����Ʈ ����� ��û
	GetAuraAbilitySystemComponent()->ServerSpendSpellPoint(SelectedAbility.AbilityTag);
}

void USpellMenuWidgetController::EquipButtonPressed()
{
	if (!AbilityInfo) return;
	if (!GetAuraAbilitySystemComponent()) return;

	// ���� ���õ� ��ų�� ��ų Ÿ��(����ú�, �нú�)�� ������
	const FGameplayTag AbilityTypeTag = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.AbilityTag).AbilityTypeTag;

	// UI�� �����ϱ� ���� ��ε�ĳ��Ʈ
	WaitForEquipSelectionDelegate.Broadcast(AbilityTypeTag);

	// ��ų ������ ��� ���� ����
	bWaitingForEquipSelection = true;
	
	// ���� ������ ��ų�� ���¸� Ȯ���ؼ� ������ �ɷ��̸� ������ ������ ��ǲ �±׸� ����
	const FGameplayTag SelectedStatusTag = GetAuraAbilitySystemComponent()->GetStatusFromAbilityTag(SelectedAbility.AbilityTag);
	if (SelectedStatusTag.MatchesTagExact(UAuraGameplayTags::Get().Abilities_Status_Equipped))
	{
		SelectedInputTag = GetAuraAbilitySystemComponent()->GetInputTagFromAbilityTag(SelectedAbility.AbilityTag);
	}
}

void USpellMenuWidgetController::SpellRowGlobePressed(const FGameplayTag& InputTag, const FGameplayTag& AbilityTypeTag)
{
	if (!AbilityInfo) return;
	// ���� ��� ���¿����� ��ų ���� ����
	if (!bWaitingForEquipSelection) return;
	
	// �����Ϸ��� ��ų Ÿ�԰� ������ ���� ������ ������ Ȯ��
	const FGameplayTag& SelectedAbilityTypeTag = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.AbilityTag).AbilityTypeTag;
	if (!SelectedAbilityTypeTag.MatchesTagExact(AbilityTypeTag)) return;


}

void USpellMenuWidgetController::ShouldEnableButton(const FGameplayTag& StatusTag, int32 SpellPoints, bool& bShouldEnableSpellPointsButton, bool& bShouldEnableEquipButton)
{
	const UAuraGameplayTags GameplayTags = UAuraGameplayTags::Get();

	// �⺻�����δ� ��� ����
	bShouldEnableSpellPointsButton = false;
	bShouldEnableEquipButton = false;

	// ��ų�� ���°� ���� ���¶�� ���� ����
	if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
	{
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}

		bShouldEnableEquipButton = true;
	}
	// ��ų�� ���°� ��� ���� ���¶�� ��ų ����Ʈ ���� ��ߵǼ� ������ �Ұ�
	else if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
	{
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}
	}
	// ��ų�� ���°� ��� ���� ���¶�� ���� ����
	else if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
	{
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}

		bShouldEnableEquipButton = true;
	}
}
