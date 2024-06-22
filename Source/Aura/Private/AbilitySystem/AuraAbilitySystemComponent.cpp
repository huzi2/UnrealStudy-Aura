// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Aura/AuraLogChannels.h"
#include "Interaction/PlayerInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	// 서버에서만 하던걸 클라에서도 수행
	// 반복하지 않도록 함
	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast();
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

FGameplayTag UAuraAbilitySystemComponent::GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (const FGameplayTag& Tag : AbilitySpec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Abilities.Status"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	// 어빌리티의 델리게이트를 수행하기전에 어빌리티를 잠금(그 사이에 제거되거나 추가되지 않는다)
	FScopedAbilityListLock ActiveScopeLock(*this);

	// 모든 활성화된 어빌리티들을 확인
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (const FGameplayTag& Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			// 어빌리티 태그가 일치하는 태그가 있을 경우 리턴
			if (Tag.MatchesTag(AbilityTag))
			{
				return &AbilitySpec;
			}
		}
	}
	return nullptr;
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetStatusFromSpec(*Spec);
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetInputTagFromSpec(*Spec);
	}
	return FGameplayTag();
}

bool UAuraAbilitySystemComponent::GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription)
{
	// 활성화된 어빌리티가 있으면 해당 어빌리티에서 설명 얻어옴
	if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec->Ability))
		{
			OutDescription = AuraAbility->GetDescription(AbilitySpec->Level);
			OutNextLevelDescription = AuraAbility->GetNextLevelDescription(AbilitySpec->Level + 1);
		}
		return true;
	}

	// 활성화된 어빌리티에서 해당 어빌리티가 없으니 어빌리티 정보들에서 탐색
	if (const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor()))
	{
		// 어빌리티 태그가 유효하지 않거나 None 상태면 설명은 비워둠
		if (!AbilityTag.IsValid() || AbilityTag.MatchesTagExact(UAuraGameplayTags::Get().Abilities_None))
		{
			OutDescription = FString();
		}
		else
		{
			// 해당 레벨에서 활성화할 수 있음을 설명에 기입
			OutDescription = UAuraGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoForTag(AbilityTag).LevelRequirement);
		}
		
		OutNextLevelDescription = FString();
		return false;
	}
	return false;
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
			// 어빌리티를 얻었으면 Equipped 태그를 추가
			AbilitySpec.DynamicAbilityTags.AddTag(UAuraGameplayTags::Get().Abilities_Status_Equipped);

			// 어빌리티 부여. 부여와 동시에 수행(GiveAbilityAndActivateOnce)도 가능
			GiveAbility(AbilitySpec);
		}
	}

	bStartupAbilitiesGiven = true;
	// 이 함수는 서버에서만 수행됨. 클라는 OnRep_ActivateAbilities()을 통해서 수행
	AbilitiesGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilites)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilites)
	{
		// 패시브 어빌리티는 인풋 태그가 필요없음. 어빌리티를 부여하자마자 활성화
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
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

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	// 어빌리티의 델리게이트를 수행하기전에 어빌리티를 잠금(그 사이에 제거되거나 추가되지 않는다)
	FScopedAbilityListLock ActiveScopeLock(*this);

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// 어빌리티에 연결된 델리게이트를 실행
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			// 어빌리티에 등록된 델리게이트가 없음
			UE_LOG(LogAura, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}

void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor() && GetAvatarActor()->Implements<UPlayerInterface>())
	{
		// 능력치 포인트가 있을 때
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
		{
			// 서버에게 해당 능력치 상승 요청
			ServerUpgradeAttribute(AttributeTag);
		}
	}
}

void UAuraAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{
	if (const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor()))
	{
		// 모든 스킬 정보에 대해서 확인
		for (const FAuraAbilityInfo& Info : AbilityInfo->GetAbilityInformation())
		{
			if (!Info.AbilityTag.IsValid()) continue;
			// 레벨에 맞지 않는 스킬은 넘어감
			if (Level < Info.LevelRequirement) continue;

			// 현재 활성화되지 않은 스킬들은 생성해서 활성화
			if (!GetSpecFromAbilityTag(Info.AbilityTag))
			{
				FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);
				// 스킬의 상태를 사용할 수 있는 상태로 변경
				AbilitySpec.DynamicAbilityTags.AddTag(UAuraGameplayTags::Get().Abilities_Status_Eligible);

				// 스킬 어빌리티 활성화
				GiveAbility(AbilitySpec);
				// 해당 어빌리티가 변경되었음을 알림(서버)
				MarkAbilitySpecDirty(AbilitySpec);
				// 해당 어빌리티가 변경되었음을 알림(클라이언트)
				ClientUpdateAbilityStatus(Info.AbilityTag, UAuraGameplayTags::Get().Abilities_Status_Eligible, 1);
			}
		}
	}
}

void UAuraAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec& AbilitySpec)
{
	// 스킬의 현재 인풋 태그를 삭제
	const FGameplayTag InputTag = GetInputTagFromSpec(AbilitySpec);
	AbilitySpec.DynamicAbilityTags.RemoveTag(InputTag);
	// 어빌리티가 변경되었음을 서버에 알림
	MarkAbilitySpecDirty(AbilitySpec);
}

void UAuraAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& InputTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilityHasSlot(AbilitySpec, InputTag))
		{
			ClearSlot(AbilitySpec);
		}
	}
}

bool UAuraAbilitySystemComponent::AbilityHasSlot(const FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& InputTag)
{
	for (const FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
	{
		if (Tag.MatchesTagExact(InputTag)) return true;
	}
	return false;
}

void UAuraAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	if (!GetAvatarActor()) return;

	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		// 스킬 포인트 사용
		if (GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
		}

		const UAuraGameplayTags GameplayTags = UAuraGameplayTags::Get();

		FGameplayTag StatusTag = GetStatusFromSpec(*AbilitySpec);
		// 스킬의 상태가 사용 가능 상태라면 언락 상태로 변경
		if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
		{
			AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Eligible);
			AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Unlocked);
			StatusTag = GameplayTags.Abilities_Status_Unlocked;
		}
		// 스킬의 상태가 장착하거나 언락된 상태라면 스킬의 레벨업
		else if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
		{
			++AbilitySpec->Level;
		}

		// 서버에게 스킬의 상태가 변경되었음을 알림
		MarkAbilitySpecDirty(*AbilitySpec);
		// 클라이언트에게 스킬의 상태가 변경되었음을 알림
		ClientUpdateAbilityStatus(AbilityTag, StatusTag, AbilitySpec->Level);
	}
}

void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		const FGameplayTag& StatusTag = GetStatusFromSpec(*AbilitySpec);

		// 스킬 상태가 이미 장착되었거나 언락된 상태만 장착 가능
		if (StatusTag == UAuraGameplayTags::Get().Abilities_Status_Equipped || StatusTag == UAuraGameplayTags::Get().Abilities_Status_Unlocked)
		{
			// 먼저 해당 슬롯의 스킬 제거
			ClearAbilitiesOfSlot(InputTag);

			// 선택한 스킬의 슬롯에서도 스킬 제거
			ClearSlot(*AbilitySpec);

			// 스킬에 현재 슬롯의 인풋 태그를 부여
			AbilitySpec->DynamicAbilityTags.AddTag(InputTag);

			// 스킬 상태가 언락이었다면
			if (StatusTag.MatchesTagExact(UAuraGameplayTags::Get().Abilities_Status_Unlocked))
			{

			}

			//const FGameplayTag& PrevInputTag = GetInputTagFromSpec(*AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	if (!GetAvatarActor()) return;

	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;

	// 스스로에게 이벤트를 전달. 어빌리티 중 이벤트를 처리하는 패시브 어빌리티 GA_ListenForEvent에서 처리 예정
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		// 능력치 포인트를 올렸으므로 하나 제거
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
	}
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	// 이펙트에 적용된 태그 관련 브로드캐스트
	EffectAssetTagsDelegate.Broadcast(TagContainer);
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
{
	// 델리게이트로 변경되었음을 알림
	AbilityStatusChangedDelegate.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}
