// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AuraGameplayTags.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	if (!AbilitySystemComponent) return;
	if (!GetAuraAttributeSet()) return;
	if (!GetAuraPlayerState()) return;

	// 태그와 어트리뷰트 인포를 연결해서 델리게이트에 바인드
	for (const auto& Pair : AuraAttributeSet->TagsToAttribute)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}

	// 능력치 포인트를 바인드
	AuraPlayerState->OnAttributePointsChangedDelegate.AddLambda(
		[this](int32 AttributePoints)
		{
			AttributePointsChangedDelegate.Broadcast(AttributePoints);
		}
	);

	// 스킬 포인트를 바인드
	AuraPlayerState->OnSpellPointsChangedDelegate.AddLambda(
		[this](int32 SpellPoints)
		{
			SpellPointsChangedDelegate.Broadcast(SpellPoints);
		}
	);
}

void UAttributeMenuWidgetController::BroadcastInitialValue()
{
	check(AttributeInfo);
	if (!GetAuraAttributeSet()) return;
	if (!GetAuraPlayerState()) return;

	// 데이터에셋을 사용해서 에셋들에게 초기값을 브로드캐스트
	for (const auto& Pair : AuraAttributeSet->TagsToAttribute)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}

	// 능력치 포인트와 스킬 포인트 초기값을 브로드캐스트
	AttributePointsChangedDelegate.Broadcast(AuraPlayerState->GetAttributePoints());
	SpellPointsChangedDelegate.Broadcast(AuraPlayerState->GetSpellPoints());
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	// FGameplayAttribute에서 값 얻어오기
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (!GetAuraAbilitySystemComponent()) return;

	AuraAbilitySystemComponent->UpgradeAttribute(AttributeTag);
}
