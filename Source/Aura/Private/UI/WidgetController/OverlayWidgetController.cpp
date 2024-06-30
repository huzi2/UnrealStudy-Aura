// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "AuraGameplayTags.h"

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	if (!AbilitySystemComponent) return;
	if (!GetAuraPlayerState()) return;
	if (!GetAuraAttributeSet()) return;
	if (!GetAuraAbilitySystemComponent()) return;

	// ����ġ�� ����Ǿ��� �� �Լ� ���ε�
	AuraPlayerState->OnXPChangedDelegate.AddUObject(this, &ThisClass::OnXPChanged);

	// ������ ������� �� �Լ� ���ε�
	AuraPlayerState->OnLevelChangedDelegate.AddLambda(
		[this](int32 NewLevel)
		{
			OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
		}
	);

	// ��Ʈ����Ʈ ��Ʈ�� ���� ������� �� ��ε�ĳ��Ʈ �ϱ����� ��Ʈ����Ʈ ��Ʈ�� �Լ��� ���ε�
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnManaChanged.Broadcast(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		}
	);

	// �޽��� ��� ��������Ʈ ����
	AuraAbilitySystemComponent->EffectAssetTagsDelegate.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
		{
			for (const FGameplayTag& Tag : AssetTags)
			{
				// Message �±��� ��쿡�� Ȯ����
				const FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(TEXT("Message"));
				if (Tag.MatchesTag(MessageTag))
				{
					if (const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag))
					{
						MessageWidgetRowDelegate.Broadcast(*Row);
					}
				}
			}
		}
	);

	// �����Ƽ ���� �� ��������Ʈ ����
	// ȣ�� ������ ���� ���� �����Ƽ���� ���޾��� ��찡 ����
	if (AuraAbilitySystemComponent->bStartupAbilitiesGiven)
	{
		BroadcastAbilityInfo();
	}
	else
	{
		AuraAbilitySystemComponent->AbilitiesGivenDelegate.AddUObject(this, &UAuraWidgetController::BroadcastAbilityInfo);
	}

	// ��ų �������� �� �Լ� ����
	AuraAbilitySystemComponent->AbilityEquippedDelegate.AddUObject(this, &ThisClass::OnAbilityEquipped);
}

void UOverlayWidgetController::BroadcastInitialValue()
{
	if (!GetAuraAttributeSet()) return;

	// ��Ʈ����Ʈ ��Ʈ�� ����� �ʱⰪ�� �����鿡�� �˸�
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& InputTag, const FGameplayTag& PrevInputTag) const
{
	// UOverlayWidgetController���� �ϴ°� ���� ȭ�鿡���� UI�� ����
	if (!AbilityInfo) return;

	const UAuraGameplayTags& GameplayTags = UAuraGameplayTags::Get();

	FAuraAbilityInfo PrevSlotInfo;
	PrevSlotInfo.AbilityTag = GameplayTags.Abilities_None;
	PrevSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;
	PrevSlotInfo.InputTag = PrevInputTag;
	// UI���� ���� ������ ��ų ���°� ����Ǿ����� �˸�
	AbilityInfoDelegate.Broadcast(PrevSlotInfo);

	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StatusTag = StatusTag;
	Info.InputTag = InputTag;
	// UI���� ������ ��ų�� ���°� ����Ǿ����� �˸�
	AbilityInfoDelegate.Broadcast(Info);
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	if (!GetAuraPlayerState()) return;

	if (const ULevelUpInfo* LevelUpInfo = AuraPlayerState->GetLevelUpInfo())
	{
		// ����ġ�� ���� ������ ���´�.
		const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
		const int32 MaxLevel = LevelUpInfo->GetMaxLevel();

		if (Level <= MaxLevel && Level > 0)
		{
			const int32 LevelUpRequirement = LevelUpInfo->GetLevelUpInfo(Level).LevelUpRequirement;
			const int32 PreviousLevelUpRequirement = LevelUpInfo->GetLevelUpInfo(Level - 1).LevelUpRequirement;

			// ���� �������� �������ִ� ����ġ��
			const int32 XPForThisLevel = NewXP - PreviousLevelUpRequirement;
			// ���� �������� �ʿ��� ����ġ��
			const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;

			// �� ����ġ�� ������ ���� ����ġ���� �ۼ��������� ����
			const float XPBarPercent = static_cast<float>(XPForThisLevel) / static_cast<float>(DeltaLevelRequirement);
			// ����ġ���� �ۼ�Ʈ�� �����ϱ����� ��ε�ĳ��Ʈ
			OnXPPercentChangedDelegate.Broadcast(XPBarPercent);
		}
	}
}
