// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/AuraWidgetController.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UAuraWidgetController::BindCallbacksToDependencies()
{
}

void UAuraWidgetController::BroadcastInitialValue()
{
}

void UAuraWidgetController::BroadcastAbilityInfo()
{
	if (!GetAuraAbilitySystemComponent()) return;
	if (!AuraAbilitySystemComponent->bStartupAbilitiesGiven) return;

	// �����Ƽ ��������Ʈ�� �����Ƽ ������(�±�, ������ ��)�� ����
	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(UAuraAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec));
			Info.InputTag = UAuraAbilitySystemComponent::GetInputTagFromSpec(AbilitySpec);
			Info.StatusTag = UAuraAbilitySystemComponent::GetStatusFromSpec(AbilitySpec);
			AbilityInfoDelegate.Broadcast(Info);
		}
	);

	// ��� �����Ƽ�� ���Ͽ� �� ��������Ʈ�� ����
	AuraAbilitySystemComponent->ForEachAbility(BroadcastDelegate);
}

AAuraPlayerController* UAuraWidgetController::GetAuraPlayerController()
{
	if (!AuraPlayerController)
	{
		AuraPlayerController = Cast<AAuraPlayerController>(PlayerController);
	}
	return AuraPlayerController;
}

AAuraPlayerState* UAuraWidgetController::GetAuraPlayerState()
{
	if (!AuraPlayerState)
	{
		AuraPlayerState = Cast<AAuraPlayerState>(PlayerState);
	}
	return AuraPlayerState;
}

UAuraAbilitySystemComponent* UAuraWidgetController::GetAuraAbilitySystemComponent()
{
	if (!AuraAbilitySystemComponent)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	}
	return AuraAbilitySystemComponent;
}

UAuraAttributeSet* UAuraWidgetController::GetAuraAttributeSet()
{
	if (!AuraAttributeSet)
	{
		AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet);
	}
	return AuraAttributeSet;
}
