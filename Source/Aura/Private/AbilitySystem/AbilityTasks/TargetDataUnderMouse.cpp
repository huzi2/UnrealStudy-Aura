// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"
#include "AbilitySystemComponent.h"
#include "Aura/Aura.h"

void UTargetDataUnderMouse::Activate()
{
	Super::Activate();

	if (!Ability) return;
	if (!Ability->GetCurrentActorInfo()) return;
	if (!AbilitySystemComponent.IsValid()) return;

	// ���� Ŭ�󿡼��� ���콺 Ÿ�� ��� Ȯ��
	if (Ability->GetCurrentActorInfo()->IsLocallyControlled())
	{
		// Ŭ�󿡼� Ȯ���� Ÿ�� ��ġ�� ������ ����
		SendMouseCursorData();
	}
	// ������ �ٸ� Ŭ����� ������ Ÿ�� �����͸� ���
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &ThisClass::OnTargetDataReplicatedCallback);

		// ��������Ʈ�� ȣ��� ������ ���
		if (!AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey))
		{
			SetWaitingOnRemotePlayerData();
		}
	}

	// ����� Vaild Data�� ���� Ŭ��� Ÿ�� �����͸� ������ ������, �ش� ��ġ�� ���� Ŭ��� ������ �˰Եȴ�.
}

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderMouse::SendMouseCursorData()
{
	if (!AbilitySystemComponent.IsValid()) return;

	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());

	if (FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit())
	{
		if (APlayerController* PlayerController = Ability->GetCurrentActorInfo()->PlayerController.Get())
		{
			// Ÿ�� �����Ϳ� Ŀ�� �浹 Ÿ�� ��ġ ����
			PlayerController->GetHitResultUnderCursor(ECC_Target, false, Data->HitResult);

			FGameplayAbilityTargetDataHandle DataHandle;
			DataHandle.Add(Data);

			// Ÿ�� �����͸� ������ ����
			AbilitySystemComponent->ServerSetReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey(), DataHandle, FGameplayTag(), AbilitySystemComponent->ScopedPredictionKey);

			// �����Ƽ�� Ȱ��ȭ�Ǿ� �־ ��ε�ĳ��Ʈ�� ������ �� �������Ʈ�� ��ε�ĳ��Ʈ
			// ���� Ŭ�� ������ ��ġ�� �ȴ�.
			if (ShouldBroadcastAbilityTaskDelegates())
			{
				ValidData.Broadcast(DataHandle);
			}
		}
	}
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	if (!AbilitySystemComponent.IsValid()) return;

	// Ÿ�� �����Ͱ� ������� �� ȣ��

	// Ŭ�󿡼� ������ Ÿ�� �����͸� ó��. ��Ʈ��ũ�� ���� �����Ͱ� Ŭ��� �Դٴ� ���̰�, �������� ����� �� ����
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());

	// ������ �����͸� ������ĳ��Ʈ
	// ������ Ŭ�󿡼� ������ ��ġ�� �� �� �ְԵȴ�.
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
