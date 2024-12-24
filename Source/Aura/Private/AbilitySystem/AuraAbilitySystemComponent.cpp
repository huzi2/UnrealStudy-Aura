// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Aura/AuraLogChannels.h"
#include "Interaction/PlayerInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Game/LoadScreenSaveGame.h"

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

void UAuraAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec& AbilitySpec)
{
	// 스킬의 현재 인풋 태그를 삭제
	const FGameplayTag InputTag = GetInputTagFromSpec(AbilitySpec);
	AbilitySpec.DynamicAbilityTags.RemoveTag(InputTag);
}

bool UAuraAbilitySystemComponent::AbilityHasSlot(const FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& InputTag)
{
	return AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag);
}

bool UAuraAbilitySystemComponent::AbilityHasAnySlot(const FGameplayAbilitySpec& AbilitySpec)
{
	return AbilitySpec.DynamicAbilityTags.HasTag(FGameplayTag::RequestGameplayTag(TEXT("InputTag")));
}

void UAuraAbilitySystemComponent::AssignSlotToAbility(FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& InputTag)
{
	ClearSlot(AbilitySpec);
	AbilitySpec.DynamicAbilityTags.AddTag(InputTag);
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
		AbilitySpec.DynamicAbilityTags.AddTag(UAuraGameplayTags::Get().Abilities_Status_Equipped);
	}
}

void UAuraAbilitySystemComponent::AddCharacterAbilitiesFromSaveData(ULoadScreenSaveGame* SaveData)
{
	if (!SaveData) return;

	for (const FSavedAbility& Data : SaveData->SavedAbilities)
	{
		const TSubclassOf<UGameplayAbility> LoadedAbilityClass = Data.GameplayAbility;
		FGameplayAbilitySpec LoadedAbilitySpec = FGameplayAbilitySpec(LoadedAbilityClass, Data.AbilityLevel);
		LoadedAbilitySpec.DynamicAbilityTags.AddTag(Data.AbilityInputTag);
		LoadedAbilitySpec.DynamicAbilityTags.AddTag(Data.AbilityStatusTag);

		if (Data.AbilityTypeTag == UAuraGameplayTags::Get().Abilities_Type_Offensive)
		{
			GiveAbility(LoadedAbilitySpec);
		}
		// 패시브면서 장착되어있던 스킬이면 바로 적용
		else if (Data.AbilityTypeTag == UAuraGameplayTags::Get().Abilities_Type_Passive)
		{
			if (Data.AbilityStatusTag.MatchesTagExact(UAuraGameplayTags::Get().Abilities_Status_Equipped))
			{
				GiveAbilityAndActivateOnce(LoadedAbilitySpec);
			}
			else
			{
				GiveAbility(LoadedAbilitySpec);
			}
		}
	}

	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	FScopedAbilityListLock ActiveScopeLock(*this);

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			// 어빌리티가 눌러졌음을 알려줌. 버튼 유지니까 계속 호출되도록함
			AbilitySpecInputPressed(AbilitySpec);

			// 어빌리티가 비활성화 상태일 때만 활성화하도록 한다.
			if (AbilitySpec.IsActive())
			{
				// 프레스 했다는 것을 서버에게 알림
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	FScopedAbilityListLock ActiveScopeLock(*this);

	// 현재 활성화할 수 있는 모든 어빌리티 중
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// 태그가 맞는 어빌리티를 가져온다.
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag) && AbilitySpec.IsActive())
		{
			// 어빌리티가 릴리즈됬음을 알림
			AbilitySpecInputReleased(AbilitySpec);
			// 릴리즈 했다는 것을 서버에게 알림
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	FScopedAbilityListLock ActiveScopeLock(*this);

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

void UAuraAbilitySystemComponent::MulticastActivatePassiveEffect_Implementation(const FGameplayTag& AbilityTag, bool bActivate)
{
	ActivatePassiveAbilityDelegate.Broadcast(AbilityTag, bActivate);
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

		const UAuraGameplayTags& GameplayTags = UAuraGameplayTags::Get();

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
		const UAuraGameplayTags& GameplayTags = UAuraGameplayTags::Get();

		// 새로 변경되기 전 인풋 태그를 기억
		const FGameplayTag& PrevInputTag = GetInputTagFromSpec(*AbilitySpec);
		const FGameplayTag& StatusTag = GetStatusFromSpec(*AbilitySpec);

		// 스킬 상태가 이미 장착되었거나 언락된 상태만 장착 가능
		if (StatusTag == GameplayTags.Abilities_Status_Equipped || StatusTag == GameplayTags.Abilities_Status_Unlocked)
		{
			// 해당 슬롯이 비어있지 않은 경우
			if (!SlotIsEmpty(InputTag))
			{
				if (FGameplayAbilitySpec* SpecWithSlot = GetSpecWithSlot(InputTag))
				{
					// 슬롯에 장착된 스킬과 장착하려는 스킬익 같으면 그냥 장착하고 끝냄
					if (AbilityTag.MatchesTagExact(GetAbilityTagFromSpec(*AbilitySpec)))
					{
						ClientEquipAbility(AbilityTag, GameplayTags.Abilities_Status_Equipped, InputTag, PrevInputTag);
						return;
					}

					// 패시브 스킬인 경우 비활성화함
					if (IsPassiveAbility(*SpecWithSlot))
					{
						// 이펙트 비활성화
						MulticastActivatePassiveEffect(GetAbilityTagFromSpec(*SpecWithSlot), false);
						// 스킬 비활성화
						DeactivatePassiveAbilityDelegate.Broadcast(GetAbilityTagFromSpec(*SpecWithSlot));
					}

					// 선택한 스킬의 슬롯에서 스킬 제거
					ClearSlot(*SpecWithSlot);
				}
			}

			// 해당 스킬이 어떤 슬롯에도 있지 않다면, 즉 활성화된 상태가 아니라면
			if (!AbilityHasAnySlot(*AbilitySpec))
			{
				// 패시브 스킬이라면 활성화
				if (IsPassiveAbility(*AbilitySpec))
				{
					// 스킬 활성화
					TryActivateAbility(AbilitySpec->Handle);
					// 이펙트 활성화
					MulticastActivatePassiveEffect(GetAbilityTagFromSpec(*AbilitySpec), true);
				}
			}

			// 해당 슬롯에 스킬 등록
			AssignSlotToAbility(*AbilitySpec, InputTag);

			// 어빌리티가 변경되었음을 서버에 알림
			MarkAbilitySpecDirty(*AbilitySpec);
		}

		ClientEquipAbility(AbilityTag, GameplayTags.Abilities_Status_Equipped, InputTag, PrevInputTag);
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

void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& InputTag, const FGameplayTag& PrevInputTag)
{
	// 델리게이트로 스킬이 장착되었음을 알림
	AbilityEquippedDelegate.Broadcast(AbilityTag, StatusTag, InputTag, PrevInputTag);
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

bool UAuraAbilitySystemComponent::SlotIsEmpty(const FGameplayTag& InputTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilityHasSlot(AbilitySpec, InputTag)) return false;
	}
	return true;
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecWithSlot(const FGameplayTag& InputTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)) return &AbilitySpec;
	}
	return nullptr;
}

bool UAuraAbilitySystemComponent::IsPassiveAbility(const FGameplayAbilitySpec& AbilitySpec) const
{
	if (const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor()))
	{
		const FGameplayTag AbilityTag = GetAbilityTagFromSpec(AbilitySpec);
		const FAuraAbilityInfo& Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
		const FGameplayTag& AbilityTypeTag = Info.AbilityTypeTag;
		return AbilityTypeTag.MatchesTagExact(UAuraGameplayTags::Get().Abilities_Type_Passive);
	}
	return false;
}
