// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "Interaction/CombatInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

UDebuffNiagaraComponent::UDebuffNiagaraComponent()
{
	bAutoActivate = false;
}

void UDebuffNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner());

	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		// 디버프 태그가 변경되었을 때 이벤트 바인드
		AbilitySystemComponent->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
	}
	// 만약 어빌리티 시스템 컴포넌트가 아직 등록되어있지 않았을 경우
	else if (CombatInterface)
	{
		// 어빌리티 시스템 컴포넌트가 등록될 때 호출되는 델리게이트에 함수 연결
		CombatInterface->GetOnAbilitySystemComponentRegisteredDelegate().AddWeakLambda(this, [this](UAbilitySystemComponent* InAbilitySystemComponent)
			{
				if (InAbilitySystemComponent)
				{
					InAbilitySystemComponent->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
				}
			}
		);
	}

	// 캐릭터가 죽었을 때 비활성화 되도록 바인드
	if (CombatInterface)
	{
		CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UDebuffNiagaraComponent::OnOwnerDeath);
	}
}

void UDebuffNiagaraComponent::DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	const bool bOwnerAlibe = IsValid(GetOwner()) && GetOwner()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(GetOwner());

	// 디버프 갯수가 1개 이상일 때 활성화
	if (NewCount > 0 && bOwnerAlibe)
	{
		Activate();
	}
	// 0개면 비활성화
	else
	{
		Deactivate();
	}
}

void UDebuffNiagaraComponent::OnOwnerDeath(AActor* DeadActor)
{
	// 컴포넌트 비활성화
	Deactivate();
}
