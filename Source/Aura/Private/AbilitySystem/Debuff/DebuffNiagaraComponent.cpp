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
		// ����� �±װ� ����Ǿ��� �� �̺�Ʈ ���ε�
		AbilitySystemComponent->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
	}
	// ���� �����Ƽ �ý��� ������Ʈ�� ���� ��ϵǾ����� �ʾ��� ���
	else if (CombatInterface)
	{
		// �����Ƽ �ý��� ������Ʈ�� ��ϵ� �� ȣ��Ǵ� ��������Ʈ�� �Լ� ����
		CombatInterface->GetOnAbilitySystemComponentRegisteredDelegate().AddWeakLambda(this, [this](UAbilitySystemComponent* InAbilitySystemComponent)
			{
				if (InAbilitySystemComponent)
				{
					InAbilitySystemComponent->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
				}
			}
		);
	}

	// ĳ���Ͱ� �׾��� �� ��Ȱ��ȭ �ǵ��� ���ε�
	if (CombatInterface)
	{
		CombatInterface->GetOnDeathDelegate().AddUObject(this, &UDebuffNiagaraComponent::OnOwnerDeath);
	}
}

void UDebuffNiagaraComponent::DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	// ����� ������ 1�� �̻��� �� Ȱ��ȭ
	if (NewCount > 0)
	{
		Activate();
	}
	// 0���� ��Ȱ��ȭ
	else
	{
		Deactivate();
	}
}

void UDebuffNiagaraComponent::OnOwnerDeath(AActor* DeadActor)
{
	// ������Ʈ ��Ȱ��ȭ
	Deactivate();
}
