// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"
#include "AuraGameplayTags.h"

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	if (!GetAuraAbilitySystemComponent()) return;
	if (!GetAuraPlayerState()) return;

	// 어빌리티 상태가 수정될 때 UI에 알려주는 함수 바인드
	GetAuraAbilitySystemComponent()->AbilityStatusChangedDelegate.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag)
	{
		if (AbilityInfo)
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			// 상태 태그 수정
			Info.StatusTag = StatusTag;
			// 모든 UI에게 해당 어빌리티가 수정되었음을 알림
			AbilityInfoDelegate.Broadcast(Info);
		}
	});

	// 플레이어 스테이트에서 스킬 포인트가 변화할 때 사용하는 델리게이트에 함수 바인드
	GetAuraPlayerState()->OnSpellPointsChangedDelegate.AddLambda([this](int32 SpellPoints)
	{
		SpellPointsChangedDelegate.Broadcast(SpellPoints);
	});
}

void USpellMenuWidgetController::BroadcastInitialValue()
{
	if (!GetAuraPlayerState()) return;

	// 기본 어빌리티 정보를 브로드캐스트해서 EquippedSpellRow의 UI를 갱신
	BroadcastAbilityInfo();

	// 스킬 포인트 초기값을 UI에 알림
	SpellPointsChangedDelegate.Broadcast(GetAuraPlayerState()->GetSpellPoints());
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	if (!GetAuraPlayerState()) return;
	if (!GetAuraAbilitySystemComponent()) return;

	const int32 SpellPoints = GetAuraPlayerState()->GetSpellPoints();

	const UAuraGameplayTags GameplayTags = UAuraGameplayTags::Get();
	const bool bTagValid = AbilityTag.IsValid();
	const bool bTagNone = AbilityTag.MatchesTag(GameplayTags.Abilities_None);
	const bool bSpecValid = GetAuraAbilitySystemComponent()->GetSpecFromAbilityTag(AbilityTag) != nullptr;

	FGameplayTag AbilityStatus;
	if (!bTagValid || bTagNone || !bSpecValid)
	{
		AbilityStatus = GameplayTags.Abilities_Status_Locked;
	}
}
