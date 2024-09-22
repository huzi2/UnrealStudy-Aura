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

	// 로컬 클라에서만 마우스 타겟 대상 확인
	if (Ability->GetCurrentActorInfo()->IsLocallyControlled())
	{
		// 클라에서 확인한 타겟 위치를 서버에 전달
		SendMouseCursorData();
	}
	// 서버나 다른 클라들은 복제된 타겟 데이터를 사용
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &ThisClass::OnTargetDataReplicatedCallback);

		// 델리게이트가 호출될 때까지 대기
		if (!AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey))
		{
			SetWaitingOnRemotePlayerData();
		}
	}

	// 결론은 Vaild Data를 통해 클라는 타겟 데이터를 서버로 보내고, 해당 위치는 로컬 클라와 서버가 알게된다.
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
			// 타겟 데이터에 커서 충돌 타겟 위치 전달
			PlayerController->GetHitResultUnderCursor(ECC_Target, false, Data->HitResult);

			FGameplayAbilityTargetDataHandle DataHandle;
			DataHandle.Add(Data);

			// 타겟 데이터를 서버로 전달
			AbilitySystemComponent->ServerSetReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey(), DataHandle, FGameplayTag(), AbilitySystemComponent->ScopedPredictionKey);

			// 어빌리티가 활성화되어 있어서 브로드캐스트가 가능할 때 블루프린트로 브로드캐스트
			// 로컬 클라가 스스로 위치를 안다.
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

	// 타겟 데이터가 복사됬을 때 호출

	// 클라에서 복제된 타겟 데이터를 처리. 네트워크를 통해 데이터가 클라로 왔다는 것이고, 로직에서 사용할 수 있음
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());

	// 복제된 데이터를 보르드캐스트
	// 서버가 클라에서 복제된 위치를 알 수 있게된다.
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
