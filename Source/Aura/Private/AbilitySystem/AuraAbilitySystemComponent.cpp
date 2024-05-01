// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Aura/AuraLogChannels.h"

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTagsDelegate.Broadcast(TagContainer);
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	// ���������� �ϴ��� Ŭ�󿡼��� ����
	// �ݺ����� �ʵ��� ��
	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast(this);
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (const FGameplayTag& Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Abilities"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (const FGameplayTag& Tag : AbilitySpec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("InputTag"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
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

	bStartupAbilitiesGiven = true;
	// �� �Լ��� ���������� �����. Ŭ��� OnRep_ActivateAbilities()�� ���ؼ� ����
	AbilitiesGivenDelegate.Broadcast(this);
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilites)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilites)
	{
		// �нú� �����Ƽ�� ��ǲ �±װ� �ʿ����. �����Ƽ�� �ο����ڸ��� Ȱ��ȭ
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
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

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	// �����Ƽ�� ��������Ʈ�� �����ϱ����� �����Ƽ�� ���(�� ���̿� ���ŵǰų� �߰����� �ʴ´�)
	FScopedAbilityListLock ActiveScopeLock(*this);

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// �����Ƽ�� ����� ��������Ʈ�� ����
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			// �����Ƽ�� ��ϵ� ��������Ʈ�� ����
			UE_LOG(LogAura, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}
