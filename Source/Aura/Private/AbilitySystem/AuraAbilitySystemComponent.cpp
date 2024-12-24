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

	// ���������� �ϴ��� Ŭ�󿡼��� ����
	// �ݺ����� �ʵ��� ��
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
	// ��ų�� ���� ��ǲ �±׸� ����
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
	// �����Ƽ�� ��������Ʈ�� �����ϱ����� �����Ƽ�� ���(�� ���̿� ���ŵǰų� �߰����� �ʴ´�)
	FScopedAbilityListLock ActiveScopeLock(*this);

	// ��� Ȱ��ȭ�� �����Ƽ���� Ȯ��
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (const FGameplayTag& Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			// �����Ƽ �±װ� ��ġ�ϴ� �±װ� ���� ��� ����
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
	// Ȱ��ȭ�� �����Ƽ�� ������ �ش� �����Ƽ���� ���� ����
	if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec->Ability))
		{
			OutDescription = AuraAbility->GetDescription(AbilitySpec->Level);
			OutNextLevelDescription = AuraAbility->GetNextLevelDescription(AbilitySpec->Level + 1);
		}
		return true;
	}

	// Ȱ��ȭ�� �����Ƽ���� �ش� �����Ƽ�� ������ �����Ƽ �����鿡�� Ž��
	if (const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor()))
	{
		// �����Ƽ �±װ� ��ȿ���� �ʰų� None ���¸� ������ �����
		if (!AbilityTag.IsValid() || AbilityTag.MatchesTagExact(UAuraGameplayTags::Get().Abilities_None))
		{
			OutDescription = FString();
		}
		else
		{
			// �ش� �������� Ȱ��ȭ�� �� ������ ���� ����
			OutDescription = UAuraGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoForTag(AbilityTag).LevelRequirement);
		}
		
		OutNextLevelDescription = FString();
		return false;
	}
	return false;
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
			// �����Ƽ�� ������� Equipped �±׸� �߰�
			AbilitySpec.DynamicAbilityTags.AddTag(UAuraGameplayTags::Get().Abilities_Status_Equipped);

			// �����Ƽ �ο�. �ο��� ���ÿ� ����(GiveAbilityAndActivateOnce)�� ����
			GiveAbility(AbilitySpec);
		}
	}

	bStartupAbilitiesGiven = true;
	// �� �Լ��� ���������� �����. Ŭ��� OnRep_ActivateAbilities()�� ���ؼ� ����
	AbilitiesGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilites)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilites)
	{
		// �нú� �����Ƽ�� ��ǲ �±װ� �ʿ����. �����Ƽ�� �ο����ڸ��� Ȱ��ȭ
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
		// �нú�鼭 �����Ǿ��ִ� ��ų�̸� �ٷ� ����
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
			// �����Ƽ�� ���������� �˷���. ��ư �����ϱ� ��� ȣ��ǵ�����
			AbilitySpecInputPressed(AbilitySpec);

			// �����Ƽ�� ��Ȱ��ȭ ������ ���� Ȱ��ȭ�ϵ��� �Ѵ�.
			if (AbilitySpec.IsActive())
			{
				// ������ �ߴٴ� ���� �������� �˸�
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	FScopedAbilityListLock ActiveScopeLock(*this);

	// ���� Ȱ��ȭ�� �� �ִ� ��� �����Ƽ ��
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// �±װ� �´� �����Ƽ�� �����´�.
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag) && AbilitySpec.IsActive())
		{
			// �����Ƽ�� ����������� �˸�
			AbilitySpecInputReleased(AbilitySpec);
			// ������ �ߴٴ� ���� �������� �˸�
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

void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor() && GetAvatarActor()->Implements<UPlayerInterface>())
	{
		// �ɷ�ġ ����Ʈ�� ���� ��
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
		{
			// �������� �ش� �ɷ�ġ ��� ��û
			ServerUpgradeAttribute(AttributeTag);
		}
	}
}

void UAuraAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{
	if (const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor()))
	{
		// ��� ��ų ������ ���ؼ� Ȯ��
		for (const FAuraAbilityInfo& Info : AbilityInfo->GetAbilityInformation())
		{
			if (!Info.AbilityTag.IsValid()) continue;
			// ������ ���� �ʴ� ��ų�� �Ѿ
			if (Level < Info.LevelRequirement) continue;

			// ���� Ȱ��ȭ���� ���� ��ų���� �����ؼ� Ȱ��ȭ
			if (!GetSpecFromAbilityTag(Info.AbilityTag))
			{
				FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);
				// ��ų�� ���¸� ����� �� �ִ� ���·� ����
				AbilitySpec.DynamicAbilityTags.AddTag(UAuraGameplayTags::Get().Abilities_Status_Eligible);

				// ��ų �����Ƽ Ȱ��ȭ
				GiveAbility(AbilitySpec);
				// �ش� �����Ƽ�� ����Ǿ����� �˸�(����)
				MarkAbilitySpecDirty(AbilitySpec);
				// �ش� �����Ƽ�� ����Ǿ����� �˸�(Ŭ���̾�Ʈ)
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
		// ��ų ����Ʈ ���
		if (GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
		}

		const UAuraGameplayTags& GameplayTags = UAuraGameplayTags::Get();

		FGameplayTag StatusTag = GetStatusFromSpec(*AbilitySpec);
		// ��ų�� ���°� ��� ���� ���¶�� ��� ���·� ����
		if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
		{
			AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Eligible);
			AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Unlocked);
			StatusTag = GameplayTags.Abilities_Status_Unlocked;
		}
		// ��ų�� ���°� �����ϰų� ����� ���¶�� ��ų�� ������
		else if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
		{
			++AbilitySpec->Level;
		}

		// �������� ��ų�� ���°� ����Ǿ����� �˸�
		MarkAbilitySpecDirty(*AbilitySpec);
		// Ŭ���̾�Ʈ���� ��ų�� ���°� ����Ǿ����� �˸�
		ClientUpdateAbilityStatus(AbilityTag, StatusTag, AbilitySpec->Level);
	}
}

void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		const UAuraGameplayTags& GameplayTags = UAuraGameplayTags::Get();

		// ���� ����Ǳ� �� ��ǲ �±׸� ���
		const FGameplayTag& PrevInputTag = GetInputTagFromSpec(*AbilitySpec);
		const FGameplayTag& StatusTag = GetStatusFromSpec(*AbilitySpec);

		// ��ų ���°� �̹� �����Ǿ��ų� ����� ���¸� ���� ����
		if (StatusTag == GameplayTags.Abilities_Status_Equipped || StatusTag == GameplayTags.Abilities_Status_Unlocked)
		{
			// �ش� ������ ������� ���� ���
			if (!SlotIsEmpty(InputTag))
			{
				if (FGameplayAbilitySpec* SpecWithSlot = GetSpecWithSlot(InputTag))
				{
					// ���Կ� ������ ��ų�� �����Ϸ��� ��ų�� ������ �׳� �����ϰ� ����
					if (AbilityTag.MatchesTagExact(GetAbilityTagFromSpec(*AbilitySpec)))
					{
						ClientEquipAbility(AbilityTag, GameplayTags.Abilities_Status_Equipped, InputTag, PrevInputTag);
						return;
					}

					// �нú� ��ų�� ��� ��Ȱ��ȭ��
					if (IsPassiveAbility(*SpecWithSlot))
					{
						// ����Ʈ ��Ȱ��ȭ
						MulticastActivatePassiveEffect(GetAbilityTagFromSpec(*SpecWithSlot), false);
						// ��ų ��Ȱ��ȭ
						DeactivatePassiveAbilityDelegate.Broadcast(GetAbilityTagFromSpec(*SpecWithSlot));
					}

					// ������ ��ų�� ���Կ��� ��ų ����
					ClearSlot(*SpecWithSlot);
				}
			}

			// �ش� ��ų�� � ���Կ��� ���� �ʴٸ�, �� Ȱ��ȭ�� ���°� �ƴ϶��
			if (!AbilityHasAnySlot(*AbilitySpec))
			{
				// �нú� ��ų�̶�� Ȱ��ȭ
				if (IsPassiveAbility(*AbilitySpec))
				{
					// ��ų Ȱ��ȭ
					TryActivateAbility(AbilitySpec->Handle);
					// ����Ʈ Ȱ��ȭ
					MulticastActivatePassiveEffect(GetAbilityTagFromSpec(*AbilitySpec), true);
				}
			}

			// �ش� ���Կ� ��ų ���
			AssignSlotToAbility(*AbilitySpec, InputTag);

			// �����Ƽ�� ����Ǿ����� ������ �˸�
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

	// �����ο��� �̺�Ʈ�� ����. �����Ƽ �� �̺�Ʈ�� ó���ϴ� �нú� �����Ƽ GA_ListenForEvent���� ó�� ����
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		// �ɷ�ġ ����Ʈ�� �÷����Ƿ� �ϳ� ����
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
	}
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	// ����Ʈ�� ����� �±� ���� ��ε�ĳ��Ʈ
	EffectAssetTagsDelegate.Broadcast(TagContainer);
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
{
	// ��������Ʈ�� ����Ǿ����� �˸�
	AbilityStatusChangedDelegate.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}

void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& InputTag, const FGameplayTag& PrevInputTag)
{
	// ��������Ʈ�� ��ų�� �����Ǿ����� �˸�
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
