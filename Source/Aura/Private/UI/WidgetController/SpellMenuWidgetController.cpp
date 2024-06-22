// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	if (!GetAuraAbilitySystemComponent()) return;
	if (!GetAuraPlayerState()) return;

	// 어빌리티 상태가 수정될 때 UI에 알려주는 함수 바인드
	GetAuraAbilitySystemComponent()->AbilityStatusChangedDelegate.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
	{
		// 현재 선택된 스킬의 상태가 변경되었다면
		if (SelectedAbility.AbilityTag.MatchesTagExact(AbilityTag))
		{
			SelectedAbility.StatusTag = StatusTag;

			// 스킬 포인트와 장착 버튼의 활성화 유무 확인
			bool bShouldEnableSpellPointsButton = false;
			bool bShouldEnableEquipButton = false;

			ShouldEnableButton(StatusTag, CurrentSpellPoints, bShouldEnableSpellPointsButton, bShouldEnableEquipButton);

			FString Description = FString();
			FString NextLevelDescription = FString();
			GetAuraAbilitySystemComponent()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);

			// 활성화 유무와 스킬 설명을 UI에게 델리게이트로 알림
			SpellGlobeSelectedDelegate.Broadcast(bShouldEnableSpellPointsButton, bShouldEnableEquipButton, Description, NextLevelDescription);
		}

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
		CurrentSpellPoints = SpellPoints;

		// 스킬 포인트와 장착 버튼의 활성화 유무 확인
		bool bShouldEnableSpellPointsButton = false;
		bool bShouldEnableEquipButton = false;

		ShouldEnableButton(SelectedAbility.StatusTag, CurrentSpellPoints, bShouldEnableSpellPointsButton, bShouldEnableEquipButton);

		FString Description = FString();
		FString NextLevelDescription = FString();
		GetAuraAbilitySystemComponent()->GetDescriptionsByAbilityTag(SelectedAbility.AbilityTag, Description, NextLevelDescription);

		// 활성화 유무와 스킬 설명을 UI에게 델리게이트로 알림
		SpellGlobeSelectedDelegate.Broadcast(bShouldEnableSpellPointsButton, bShouldEnableEquipButton, Description, NextLevelDescription);
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

	// 스킬 장착을 기다리는 상태라면
	if (bWaitingForEquipSelection && AbilityInfo)
	{
		const FGameplayTag AbilityTypeTag = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.AbilityTag).AbilityTypeTag;
		// 다른 스킬을 선택했거나 다시 선택하면 스킬 장착 기다리는 상태도 취소
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
	// 어빌리티가 유효하지 않음
	if (!bTagValid || bTagNone || !bSpecValid)
	{
		StatusTag = GameplayTags.Abilities_Status_Locked;
	}
	// 어빌리티가 유효함
	else
	{
		StatusTag = GetAuraAbilitySystemComponent()->GetStatusFromSpec(*AbilitySpec);
	}

	// 현재 선택되어있는 스킬의 상태가 변경되었을 때 델리게이트에서 수정하기위해 변수를 저장
	SelectedAbility.AbilityTag = AbilityTag;
	SelectedAbility.StatusTag = StatusTag;
	CurrentSpellPoints = SpellPoints;

	// 스킬 포인트와 장착 버튼의 활성화 유무 확인
	bool bShouldEnableSpellPointsButton = false;
	bool bShouldEnableEquipButton = false;

	ShouldEnableButton(StatusTag, SpellPoints, bShouldEnableSpellPointsButton, bShouldEnableEquipButton);

	FString Description = FString();
	FString NextLevelDescription = FString();
	GetAuraAbilitySystemComponent()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);

	// 버튼 활성화 유무와 스킬 설명을 UI에게 델리게이트로 알림
	SpellGlobeSelectedDelegate.Broadcast(bShouldEnableSpellPointsButton, bShouldEnableEquipButton, Description, NextLevelDescription);
}

void USpellMenuWidgetController::SpellGlobeDeselect()
{
	// 스킬 장착을 기다리는 상태라면
	if (bWaitingForEquipSelection && AbilityInfo)
	{
		const FGameplayTag AbilityTypeTag = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.AbilityTag).AbilityTypeTag;
		// 스킬 선택이 취소되었으니 스킬 장착 기다리는 상태도 취소
		StopWaitingForEquipDelegate.Broadcast(AbilityTypeTag);
		bWaitingForEquipSelection = false;
	}

	SelectedAbility.AbilityTag = UAuraGameplayTags::Get().Abilities_None;
	SelectedAbility.StatusTag = UAuraGameplayTags::Get().Abilities_Status_Locked;

	// 버튼 활성화 유무와 스킬 설명 초기화
	SpellGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());
}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
	if (!GetAuraAbilitySystemComponent()) return;

	// 서버에게 해당 스킬에게 스킬 포인트 사용을 요청
	GetAuraAbilitySystemComponent()->ServerSpendSpellPoint(SelectedAbility.AbilityTag);
}

void USpellMenuWidgetController::EquipButtonPressed()
{
	if (!AbilityInfo) return;
	if (!GetAuraAbilitySystemComponent()) return;

	// 현재 선택된 스킬의 스킬 타입(오펜시브, 패시브)을 가져옴
	const FGameplayTag AbilityTypeTag = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.AbilityTag).AbilityTypeTag;

	// UI에 적용하기 위해 브로드캐스트
	WaitForEquipSelectionDelegate.Broadcast(AbilityTypeTag);

	// 스킬 장착을 대기 중인 상태
	bWaitingForEquipSelection = true;
	
	// 현재 선택한 스킬의 상태를 확인해서 장착된 능력이면 장착된 슬롯의 인풋 태그를 얻음
	const FGameplayTag SelectedStatusTag = GetAuraAbilitySystemComponent()->GetStatusFromAbilityTag(SelectedAbility.AbilityTag);
	if (SelectedStatusTag.MatchesTagExact(UAuraGameplayTags::Get().Abilities_Status_Equipped))
	{
		SelectedInputTag = GetAuraAbilitySystemComponent()->GetInputTagFromAbilityTag(SelectedAbility.AbilityTag);
	}
}

void USpellMenuWidgetController::SpellRowGlobePressed(const FGameplayTag& InputTag, const FGameplayTag& AbilityTypeTag)
{
	if (!AbilityInfo) return;
	// 장착 대기 상태에서만 스킬 장착 가능
	if (!bWaitingForEquipSelection) return;
	
	// 장착하려는 스킬 타입과 선택한 구슬 슬롯이 같은지 확인
	const FGameplayTag& SelectedAbilityTypeTag = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.AbilityTag).AbilityTypeTag;
	if (!SelectedAbilityTypeTag.MatchesTagExact(AbilityTypeTag)) return;


}

void USpellMenuWidgetController::ShouldEnableButton(const FGameplayTag& StatusTag, int32 SpellPoints, bool& bShouldEnableSpellPointsButton, bool& bShouldEnableEquipButton)
{
	const UAuraGameplayTags GameplayTags = UAuraGameplayTags::Get();

	// 기본적으로는 잠긴 상태
	bShouldEnableSpellPointsButton = false;
	bShouldEnableEquipButton = false;

	// 스킬의 상태가 장착 상태라면 장착 가능
	if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
	{
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}

		bShouldEnableEquipButton = true;
	}
	// 스킬의 상태가 사용 가능 상태라면 스킬 포인트 먼저 써야되서 장착은 불가
	else if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
	{
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}
	}
	// 스킬의 상태가 잠금 해제 상태라면 장착 가능
	else if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
	{
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}

		bShouldEnableEquipButton = true;
	}
}
