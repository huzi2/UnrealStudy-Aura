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

	// �����Ƽ ���°� ������ �� UI�� �˷��ִ� �Լ� ���ε�
	GetAuraAbilitySystemComponent()->AbilityStatusChangedDelegate.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag)
	{
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
