// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/AsyncTasks/WaitCooldownChange.h"
#include "AbilitySystemComponent.h"

UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* InAbilitySystemComponent, const FGameplayTag& InCooldownTag)
{
	UWaitCooldownChange* WaitCooldownChange = NewObject<UWaitCooldownChange>();
	if (WaitCooldownChange)
	{
		if (!IsValid(InAbilitySystemComponent) || !InCooldownTag.IsValid())
		{
			WaitCooldownChange->EndTask();
			return nullptr;
		}

		WaitCooldownChange->AbilitySystemComponent = InAbilitySystemComponent;
		WaitCooldownChange->CooldownTag = InCooldownTag;

		// ��Ÿ���� ���۵� �� ����� �Լ��� ���ε�
		InAbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCooldownChange, &ThisClass::OnActiveEffectAdded);

		// ��Ÿ���� ������ ��(���ŵ� ��) ����� �Լ��� ���ε�
		InAbilitySystemComponent->RegisterGameplayTagEvent(InCooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(WaitCooldownChange, &ThisClass::CooldownTagChanged);
	}
	return WaitCooldownChange;
}

void UWaitCooldownChange::EndTask()
{
	if (IsValid(AbilitySystemComponent))
	{
		// �ݹ� ����
		AbilitySystemComponent->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

		// ��ü�� �ı� �غ�Ǿ����� �˸�
		SetReadyToDestroy();
		// ������ �÷��� ������� �˸�
		MarkAsGarbage();
	}
}

void UWaitCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetAbilitySystemComponent, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	if (!AbilitySystemComponent) return;

	// ����Ʈ�� ��� ���� �±׸� ������
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);

	// ����Ʈ�� ��� �ο��� �±׸� ������
	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(GrantedTags);

	// �� �±׵� �߿� ��ٿ� �±װ� ���ԵǾ��ٸ�
	if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))
	{
		// ��ٿ� �±׸� �����ϴ� ������ ����
		const FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
		// ������ �´� ��� ����Ʈ�� ���� �ð��� �����´�.
		const TArray<float> TimesRemaining = AbilitySystemComponent->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
		if (TimesRemaining.Num() > 0)
		{
			// ��� ����Ʈ�� �ð� �� ���� �� �ð��� ��������(���� ��Ÿ���� ������ �س��ٸ�)
			const float TimeRemaining = FMath::Max(TimesRemaining);
			CooldownStart.Broadcast(TimeRemaining);
		}
	}
}

void UWaitCooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
	if (NewCount == 0)
	{
		CooldownEnd.Broadcast(0.f);
	}
}
