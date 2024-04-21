// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void UOverlayWidgetController::BroadcastInitialValue()
{
	// 어트리뷰트 세트에 저장된 초기값을 위젯들에게 알림
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	// 어트리뷰트 세트의 값이 변경됬을 때 브로드캐스트 하기위해 어트리뷰트 세트와 함수를 바인드
	if (!AbilitySystemComponent) return;

	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

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
			OnInitializeStartupAbilities(AuraAbilitySystemComponent);
		}
		else
		{
			AuraAbilitySystemComponent->AbilitiesGivenDelegate.AddUObject(this, &ThisClass::OnInitializeStartupAbilities);
		}
	}
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

	// 모든 어빌리티에 대하여 위 델리게이트를 수행
	AuraAbilitySystemComponent->ForEachAbility(BroadcastDelegate);
}
