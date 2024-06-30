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

	// 경험치가 변경되었을 때 함수 바인드
	AuraPlayerState->OnXPChangedDelegate.AddUObject(this, &ThisClass::OnXPChanged);

	// 레벨이 변경됬을 때 함수 바인드
	AuraPlayerState->OnLevelChangedDelegate.AddLambda(
		[this](int32 NewLevel)
		{
			OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
		}
	);

	// 어트리뷰트 세트의 값이 변경됬을 때 브로드캐스트 하기위해 어트리뷰트 세트와 함수를 바인드
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

	// 메시지 출력 델리게이트 연결
	AuraAbilitySystemComponent->EffectAssetTagsDelegate.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
		{
			for (const FGameplayTag& Tag : AssetTags)
			{
				// Message 태그인 경우에만 확인함
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

	// 어빌리티 얻을 때 델리게이트 연결
	// 호출 시점에 따라 시작 어빌리티들을 못받았을 경우가 있음
	if (AuraAbilitySystemComponent->bStartupAbilitiesGiven)
	{
		BroadcastAbilityInfo();
	}
	else
	{
		AuraAbilitySystemComponent->AbilitiesGivenDelegate.AddUObject(this, &UAuraWidgetController::BroadcastAbilityInfo);
	}

	// 스킬 장착했을 때 함수 연결
	AuraAbilitySystemComponent->AbilityEquippedDelegate.AddUObject(this, &ThisClass::OnAbilityEquipped);
}

void UOverlayWidgetController::BroadcastInitialValue()
{
	if (!GetAuraAttributeSet()) return;

	// 어트리뷰트 세트에 저장된 초기값을 위젯들에게 알림
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& InputTag, const FGameplayTag& PrevInputTag) const
{
	// UOverlayWidgetController에서 하는건 게임 화면에서의 UI를 수정
	if (!AbilityInfo) return;

	const UAuraGameplayTags& GameplayTags = UAuraGameplayTags::Get();

	FAuraAbilityInfo PrevSlotInfo;
	PrevSlotInfo.AbilityTag = GameplayTags.Abilities_None;
	PrevSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;
	PrevSlotInfo.InputTag = PrevInputTag;
	// UI에게 이전 슬롯의 스킬 상태가 변경되었음을 알림
	AbilityInfoDelegate.Broadcast(PrevSlotInfo);

	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StatusTag = StatusTag;
	Info.InputTag = InputTag;
	// UI에게 장착할 스킬의 상태가 변경되었음을 알림
	AbilityInfoDelegate.Broadcast(Info);
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	if (!GetAuraPlayerState()) return;

	if (const ULevelUpInfo* LevelUpInfo = AuraPlayerState->GetLevelUpInfo())
	{
		// 경험치를 통해 레벨을 얻어온다.
		const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
		const int32 MaxLevel = LevelUpInfo->GetMaxLevel();

		if (Level <= MaxLevel && Level > 0)
		{
			const int32 LevelUpRequirement = LevelUpInfo->GetLevelUpInfo(Level).LevelUpRequirement;
			const int32 PreviousLevelUpRequirement = LevelUpInfo->GetLevelUpInfo(Level - 1).LevelUpRequirement;

			// 현재 레벨에서 가지고있는 경험치량
			const int32 XPForThisLevel = NewXP - PreviousLevelUpRequirement;
			// 현재 레벨에서 필요한 경험치량
			const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;

			// 두 경험치를 나눠서 현재 경험치바의 퍼센테이지를 구함
			const float XPBarPercent = static_cast<float>(XPForThisLevel) / static_cast<float>(DeltaLevelRequirement);
			// 경험치바의 퍼센트를 적용하기위해 브로드캐스트
			OnXPPercentChangedDelegate.Broadcast(XPBarPercent);
		}
	}
}
