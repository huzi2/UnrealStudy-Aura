// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Passive/PassiveNiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Interaction/CombatInterface.h"
#include "AuraGameplayTags.h"

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	bAutoActivate = false;
}

void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UAuraAbilitySystemComponent* AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		AuraAbilitySystemComponent->ActivatePassiveAbilityDelegate.AddUObject(this, &ThisClass::OnPassiveAbilityActivate);
		
		ActivateIfEquipped(AuraAbilitySystemComponent);
	}
	// 아직 AuraAbilitySystemComponent를 생성하지 못했다면
	else if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		CombatInterface->GetOnAbilitySystemComponentRegisteredDelegate().AddLambda([this](UAuraAbilitySystemComponent* AuraAbilitySystemComponent)
			{
				AuraAbilitySystemComponent->ActivatePassiveAbilityDelegate.AddUObject(this, &ThisClass::OnPassiveAbilityActivate);

				ActivateIfEquipped(AuraAbilitySystemComponent);
			});
	}
}

void UPassiveNiagaraComponent::OnPassiveAbilityActivate(const FGameplayTag& AbilityTag, bool bActivate)
{
	if (AbilityTag.MatchesTagExact(PassiveSpellTag))
	{
		if (bActivate && !IsActive())
		{
			Activate();
		}
		else
		{
			Deactivate();
		}
	}
}

void UPassiveNiagaraComponent::ActivateIfEquipped(UAuraAbilitySystemComponent* AuraAbilitySystemComponent)
{
	const bool bStartupAbilitiesActivated = AuraAbilitySystemComponent->bStartupAbilitiesGiven;
	if (bStartupAbilitiesActivated && AuraAbilitySystemComponent->GetStatusFromAbilityTag(PassiveSpellTag) == UAuraGameplayTags::Get().Abilities_Status_Equipped)
	{
		Activate();
	}
}
