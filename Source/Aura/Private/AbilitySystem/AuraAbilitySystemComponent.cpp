// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTags.Broadcast(TagContainer);
}

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	// 이펙트가 적용될 때 델리게이트
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &ThisClass::ClientEffectApplied);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilites)
{
	// 캐릭터가 처음부터 가지고 있는 어빌리티들을 어빌리티 시스템 컴포넌트에 등록
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilites)
	{
		// 어빌리티 스펙을 생성. 이는 클라들에게 레플리케이션됨
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);

		// 커스텀 어빌리티 클래스로 캐스트
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			// 어빌리티의 인풋 태그를 어빌리티 스펙에 등록
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);

			// 어빌리티 부여. 부여와 동시에 수행(GiveAbilityAndActivateOnce)도 가능
			GiveAbility(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	// 현재 활성화할 수 있는 모든 어빌리티 중
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// 태그가 맞는 어빌리티를 가져온다.
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			// 어빌리티가 릴리즈됬음을 알림
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			// 어빌리티가 눌러졌음을 알려줌. 버튼 유지니까 계속 호출되도록함
			AbilitySpecInputPressed(AbilitySpec);

			// 어빌리티가 비활성화 상태일 때만 활성화하도록 한다.
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}
