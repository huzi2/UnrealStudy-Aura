// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/Data/LevelUpInfo.h"

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	if (!AbilitySystemComponent) return;

	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

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

	if (UAuraAbilitySystemComponent* AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
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
			OnInitializeStartupAbilities(AuraAbilitySystemComponent);
		}
		else
		{
			AuraAbilitySystemComponent->AbilitiesGivenDelegate.AddUObject(this, &ThisClass::OnInitializeStartupAbilities);
		}
	}
}

void UOverlayWidgetController::BroadcastInitialValue()
{
	// ��Ʈ����Ʈ ��Ʈ�� ����� �ʱⰪ�� �����鿡�� �˸�
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraAbilitySystemComponent)
{
	if (!AuraAbilitySystemComponent) return;
	if (!AuraAbilitySystemComponent->bStartupAbilitiesGiven) return;

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(UAuraAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec));
			Info.InputTag = UAuraAbilitySystemComponent::GetInputTagFromSpec(AbilitySpec);
			AbilityInfoDelegate.Broadcast(Info);
		}
	);

	// ��� �����Ƽ�� ���Ͽ� �� ��������Ʈ�� ����
	AuraAbilitySystemComponent->ForEachAbility(BroadcastDelegate);
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP) const
{
	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);

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
