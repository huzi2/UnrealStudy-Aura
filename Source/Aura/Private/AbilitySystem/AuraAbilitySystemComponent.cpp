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
	// ����Ʈ�� ����� �� ��������Ʈ
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &ThisClass::ClientEffectApplied);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilites)
{
	// ĳ���Ͱ� ó������ ������ �ִ� �����Ƽ���� �����Ƽ �ý��� ������Ʈ�� ���
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilites)
	{
		// �����Ƽ ������ ����. �̴� Ŭ��鿡�� ���ø����̼ǵ�
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);

		// Ŀ���� �����Ƽ Ŭ������ ĳ��Ʈ
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			// �����Ƽ�� ��ǲ �±׸� �����Ƽ ���忡 ���
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);

			// �����Ƽ �ο�. �ο��� ���ÿ� ����(GiveAbilityAndActivateOnce)�� ����
			GiveAbility(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	// ���� Ȱ��ȭ�� �� �ִ� ��� �����Ƽ ��
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// �±װ� �´� �����Ƽ�� �����´�.
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			// �����Ƽ�� ����������� �˸�
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
			// �����Ƽ�� ���������� �˷���. ��ư �����ϱ� ��� ȣ��ǵ�����
			AbilitySpecInputPressed(AbilitySpec);

			// �����Ƽ�� ��Ȱ��ȭ ������ ���� Ȱ��ȭ�ϵ��� �Ѵ�.
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}
