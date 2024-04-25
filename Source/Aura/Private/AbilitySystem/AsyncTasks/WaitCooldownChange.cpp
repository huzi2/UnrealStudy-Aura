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

		// 쿨타임이 시작될 때 사용할 함수를 바인드
		InAbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCooldownChange, &ThisClass::OnActiveEffectAdded);

		// 쿨타임이 끝났을 때(제거될 때) 사용할 함수를 바인드
		InAbilitySystemComponent->RegisterGameplayTagEvent(InCooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(WaitCooldownChange, &ThisClass::CooldownTagChanged);
	}
	return WaitCooldownChange;
}

void UWaitCooldownChange::EndTask()
{
	if (IsValid(AbilitySystemComponent))
	{
		// 콜백 제거
		AbilitySystemComponent->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

		// 객체가 파괴 준비되었음을 알림
		SetReadyToDestroy();
		// 가비지 컬렉션 대상임을 알림
		MarkAsGarbage();
	}
}

void UWaitCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetAbilitySystemComponent, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	if (!AbilitySystemComponent) return;

	// 이펙트의 모든 에셋 태그를 가져옴
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);

	// 이펙트의 모든 부여된 태그를 가져옴
	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(GrantedTags);

	// 위 태그들 중에 쿨다운 태그가 포함되었다면
	if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))
	{
		// 쿨다운 태그를 포함하는 쿼리를 생성
		const FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
		// 쿼리에 맞는 모든 이펙트의 남은 시간을 가져온다.
		const TArray<float> TimesRemaining = AbilitySystemComponent->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
		if (TimesRemaining.Num() > 0)
		{
			// 모든 이펙트의 시간 중 가장 긴 시간을 기준으로(만약 쿨타임을 여러개 해놨다면)
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
