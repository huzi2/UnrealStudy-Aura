// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "AuraGameplayTags.h"
#include "Interaction/CombatInterface.h"
#include "Player/AuraPlayerController.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/PlayerInterface.h"
#include "AuraAbilityTypes.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	const UAuraGameplayTags& GameplayTags = UAuraGameplayTags::Get();

	// �����÷��� �±׿� �´� GetAttribute() �Լ��� ����
	// ���� ��Ʈ�ѷ����� �±׿� ���� �Լ��� ���ε��� �� �ݺ��۾��� ���ϰ� �ϱ� ����
	TagsToAttribute.Add(GameplayTags.Attribute_Primary_Strength, GetStrengthAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Primary_Resilience, GetResilienceAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Primary_Vigor, GetVigorAttribute);

	TagsToAttribute.Add(GameplayTags.Attribute_Secondary_Armor, GetArmorAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Secondary_BlockChance, GetBlockChanceAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Secondary_ManaRegeneration, GetManaRegenerationAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Secondary_MaxMana, GetMaxManaAttribute);

	TagsToAttribute.Add(GameplayTags.Attribute_Resistance_Fire, GetFireResistanceAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Resistance_Lightning, GetLightningResistanceAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Resistance_Arcane, GetArcaneResistanceAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Resistance_Physical, GetPhysicalResistanceAttribute);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// ���� ������ ���ʴ�� �ؾ���. ���� ��� Armor�� Resilience�� ������ �����Ƿ� ���߿� �����Ǿ���
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// ����Ʈ�� ������ ����Ǳ� ���� ����. �������� �ùٸ� ������ Ŭ�����Ѵ�.
	// ������ ���� ���� Ŭ���������� �ʰ�, �����Ǽ� ��ȯ�ϴ� ���� Ŭ�����Ѵ�.
	// �׷��� ������ Ŭ������ PostGameplayEffectExecute()���� �����ؾ��Ѵ�.
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// ����Ʈ�� ������ ����� �Ŀ� ����. �ҽ��� Ÿ�� ���� ������ �� ��ó���� �� �� �ִ�.
	FEffectProperties Props;
	SetEffectProperties(Data, Props);

	// Ÿ�� ĳ���Ͱ� �׾��ִٸ� ����
	if (Props.TargetCharacter && Props.TargetCharacter->Implements<UCombatInterface>())
	{
		if (ICombatInterface::Execute_IsDead(Props.TargetCharacter))
		{
			return;
		}
	}

	// PreAttributeChange()���� �ߴ� Ŭ������ ���⼭�� �ؼ� ���⼭�� ���� ���� Ŭ�������ش�.
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	// �������� ó��. ��Ÿ ������(���ø�����Ʈ��������)�̹Ƿ� ���������� ó���ȴ�
	else if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(Props);
	}
	// ����ġ�� ó��
	else if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
		HandleIncomingXP(Props);
	}
}

void UAuraAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	// �ɷ�ġ ��ȭ ��ó��

	// ���������� �� ü�°� ������ �ִ�� ȸ��
	if (Attribute == GetMaxHealthAttribute() && bTopOffHelath)
	{
		SetHealth(GetMaxHealth());
		bTopOffHelath = false;
	}
	if (Attribute == GetMaxManaAttribute() && bTopOffMana)
	{
		SetMana(GetMaxMana());
		bTopOffMana = false;
	}
}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	// �ҽ�(����Ʈ�� ����Ų ����, ȭ������ ���� ����) ���� ���� ��������
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (!Props.SourceController && Props.SourceAvatarActor)
		{
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
		if (Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	// Ÿ��(����Ʈ�� ���� �÷��̾� ���� ����) ���� ���� ��������
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}

void UAuraAttributeSet::HandleIncomingDamage(const FEffectProperties& Props)
{
	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f);
	if (LocalIncomingDamage > 0.f)
	{
		const float NewHealth = GetHealth() - LocalIncomingDamage;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

		// �ش� Ŭ���̾�Ʈ���� Ÿ�� �Ӹ� ���� ������ UI ǥ��
		// Ŀ���� �����÷��� ����Ʈ ���ؽ�Ʈ���� ��ϰ� ġ��Ÿ Ȯ��
		const bool bBlockedHit = UAuraAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);
		const bool bCriticalHit = UAuraAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);
		ShowFloatingText(Props, LocalIncomingDamage, bBlockedHit, bCriticalHit);

		// ������� �ɷȴٸ�
		if (UAuraAbilitySystemLibrary::IsSuccessfulDebuff(Props.EffectContextHandle))
		{
			Debuff(Props);
		}

		const bool bFatal = NewHealth <= 0.f;
		// ���� �ʾ����� �ǰ� �����Ƽ Ȱ��ȭ
		if (!bFatal)
		{
			// Effect.HitReact �±װ� ���� �����Ƽ�� Ȱ��ȭ
			FGameplayTagContainer TagContainer;
			TagContainer.AddTag(UAuraGameplayTags::Get().Effects_HitReact);
			Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
		}
		else
		{
			// �������̽��� ���� ���� ó��(���׵��� ����)
			if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor))
			{
				CombatInterface->Die();
			}

			// �׿��ٸ� ����ġ�� ����
			SendXPEvent(Props);
		}
	}
}

void UAuraAttributeSet::Debuff(const FEffectProperties& Props)
{
	const UAuraGameplayTags& GameplayTags = UAuraGameplayTags::Get();

	// ����Ʈ �ڵ� ��ü ����
	FGameplayEffectContextHandle EffectContextHandle = Props.SourceASC->MakeEffectContext();
	// ����Ʈ �ڵ鿡 ��� ��ü ����
	EffectContextHandle.AddSourceObject(Props.SourceAvatarActor);

	// ����Ʈ ���ؽ�Ʈ �ڵ鿡�� ����� ���� ������ ������
	const FGameplayTag DamageTypeTag = UAuraAbilitySystemLibrary::GetDamageType(Props.EffectContextHandle);
	const FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"), *DamageTypeTag.ToString());
	const float DebuffDamage = UAuraAbilitySystemLibrary::GetDebuffDamage(Props.EffectContextHandle);
	const float DebuffDuration = UAuraAbilitySystemLibrary::GetDebuffDuration(Props.EffectContextHandle);
	const float DebuffFrequency = UAuraAbilitySystemLibrary::GetDebuffFrequency(Props.EffectContextHandle);

	// ����� ������ �ٿ��� ����Ʈ ��ü ����
	if (UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(DebuffName)))
	{
		// ������� ���ӵ�
		Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
		// ����� ���ӽð�
		Effect->DurationMagnitude = FScalableFloat(DebuffDuration);
		// ����� �ֱ�
		Effect->Period = DebuffFrequency;

		// ������ Ÿ�Կ� ���� ����� �±׸� �־ ������� �ߺ����� �ʵ��� ����
		UTargetTagsGameplayEffectComponent& AssetTagsComponent = Effect->FindOrAddComponent<UTargetTagsGameplayEffectComponent>();
		FInheritedTagContainer InheritedTagContainer;
		InheritedTagContainer.Added.AddTag(GameplayTags.DamageTypesToDebuffs[DamageTypeTag]);
		AssetTagsComponent.SetAndApplyTargetTagChanges(InheritedTagContainer);

		// ����Ʈ ��ü�� �������� ����
		Effect->StackingType = EGameplayEffectStackingType::AggregateBySource;
		// ������� �ϳ��� ����
		Effect->StackLimitCount = 1;

		// ����Ʈ�� ������̾� ����
		const int32 Index = Effect->Modifiers.Num();
		Effect->Modifiers.Add(FGameplayModifierInfo());
		FGameplayModifierInfo& ModifierInfo = Effect->Modifiers[Index];

		// ������̾ ����� ������ ����
		ModifierInfo.ModifierMagnitude = FScalableFloat(DebuffDamage);
		ModifierInfo.ModifierOp = EGameplayModOp::Additive;
		ModifierInfo.Attribute = UAuraAttributeSet::GetIncomingDamageAttribute();

		// 1������ ����Ʈ ���� ����
		if (FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(Effect, EffectContextHandle, 1.f))
		{
			if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(MutableSpec->GetContext().Get()))
			{
				// �±� ����
				TSharedPtr<FGameplayTag> DebuffDamageTypeTag = MakeShareable(new FGameplayTag(DamageTypeTag));
				AuraContext->SetDamageType(DebuffDamageTypeTag);
			}
			
			// Ÿ�ٿ��� ����Ʈ ����
			Props.TargetASC->ApplyGameplayEffectSpecToSelf(*MutableSpec);
		}
	}
}

void UAuraAttributeSet::HandleIncomingXP(const FEffectProperties& Props)
{
	const float LocalIncomingXP = GetIncomingXP();
	SetIncomingXP(0.f);

	// �������̽��� ���� �÷��̾�� ����ġ ����
	if (Props.SourceCharacter->Implements<UPlayerInterface>() && Props.SourceCharacter->Implements<UCombatInterface>())
	{
		const int32 CurrentLevel = ICombatInterface::Execute_GetPlayerLevel(Props.SourceCharacter);
		const int32 CurrentXP = IPlayerInterface::Execute_GetXP(Props.SourceCharacter);

		// ����ġ�� �������� �� ����
		const int32 NewLevel = IPlayerInterface::Execute_FindLevelForXP(Props.SourceCharacter, CurrentXP + LocalIncomingXP);
		const int32 NumLevelUps = NewLevel - CurrentLevel;
		// �������� �ߴٸ�
		if (NumLevelUps > 0)
		{
			// ������ �� ���� �ɷ�ġ�� ��ų����Ʈ�� �����´�.
			const int32 AttributePointsReward = IPlayerInterface::Execute_GetAttributePointReward(Props.SourceCharacter, CurrentLevel);
			const int32 SpellPointsReward = IPlayerInterface::Execute_GetSpellPointReward(Props.SourceCharacter, CurrentLevel);

			// �������ϸ鼭 ����, �ɷ�ġ, ��ų����Ʈ�� ���
			IPlayerInterface::Execute_AddToPlayerLevel(Props.SourceCharacter, NumLevelUps);
			IPlayerInterface::Execute_AddToAttributePoints(Props.SourceCharacter, AttributePointsReward);
			IPlayerInterface::Execute_AddToSpellPoints(Props.SourceCharacter, SpellPointsReward);

			// ü�°� ���� ȸ��(������ ���� Max�� �޶����Ƿ� ��ó������ ����)
			bTopOffHelath = true;
			bTopOffMana = true;

			IPlayerInterface::Execute_LevelUp(Props.SourceCharacter);
		}

		IPlayerInterface::Execute_AddToXP(Props.SourceCharacter, LocalIncomingXP);
	}
}

void UAuraAttributeSet::ShowFloatingText(const FEffectProperties& Props, float Damage, bool bBlockedHit, bool bCriticalHit) const
{
	if (Props.SourceCharacter != Props.TargetCharacter)
	{
		// �÷��̾ ������ �� ������ �˾�
		if (AAuraPlayerController* PlayerController = Cast<AAuraPlayerController>(Props.SourceCharacter->Controller))
		{
			PlayerController->ClientShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
			return;
		}
		// ���� ���� ������ �� ������ �˾�
		if (AAuraPlayerController* PlayerController = Cast<AAuraPlayerController>(Props.TargetCharacter->Controller))
		{
			PlayerController->ClientShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
		}
	}
}

void UAuraAttributeSet::SendXPEvent(const FEffectProperties& Props)
{
	if (Props.TargetCharacter->Implements<UCombatInterface>())
	{
		// Ÿ���� ������ ������ ��� ����ġ ������ Ȯ��
		const int32 TargetLevel = ICombatInterface::Execute_GetPlayerLevel(Props.TargetCharacter);
		const ECharacterClass TargetClass = ICombatInterface::Execute_GetCharacterClass(Props.TargetCharacter);
		const int32 XPReward = UAuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(Props.TargetCharacter, TargetClass, static_cast<float>(TargetLevel));

		// �нú� �����Ƽ�� �����ϵ��� �±׸� ����
		const UAuraGameplayTags& GameplayTags = UAuraGameplayTags::Get();
		// �нú� �����Ƽ���� Ȯ���� �±׿� ��ġ�� �����ؼ� ������.
		FGameplayEventData Payload;
		Payload.EventTag = GameplayTags.Attribute_Meta_IncomingXP;
		Payload.EventMagnitude = static_cast<float>(XPReward);

		// �ҽ� ���Ϳ��� �����÷��� �̺�Ʈ�� ����(�нú� �����Ƽ���� ������ ��)
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, GameplayTags.Attribute_Meta_IncomingXP, Payload);
	}
}

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldVigor);
}

void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldArmor);
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldBlockChance);
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}

void UAuraAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, FireResistance, OldFireResistance);
}

void UAuraAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, LightningResistance, OldLightningResistance);
}

void UAuraAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArcaneResistance, OldArcaneResistance);
}

void UAuraAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, PhysicalResistance, OldPhysicalResistance);
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMana);
}
