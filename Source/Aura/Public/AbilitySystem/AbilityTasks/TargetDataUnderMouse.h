// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderMouse.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle);

/**
 * 마우스 아래 타겟을 찾는 어빌리티 태스크
 */
UCLASS()
class AURA_API UTargetDataUnderMouse : public UAbilityTask
{
	GENERATED_BODY()
	
private:
	virtual void Activate() final;

private:
	// 블루프린트에서 TargetDataUnderMouse의 이름으로 호출되고, OwningAbility 핀을 숨기고 self로 기본값을 넣음. BlueprintInternalUseOnly는 해당 함수가 블루프린트 내부에서만 사용되어야한다는 뜻
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "TargetDataUnderMouse", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UTargetDataUnderMouse* CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility);

private:
	// 클라에서 확인한 타겟 위치를 서버에 전달
	void SendMouseCursorData();

	// 타겟 데이터가 복사됬을 때 호출
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);

private:
	// 블루프린트에서 CreateTargetDataUnderMouse() 함수의 핀으로 자동 연결된다.
	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignature ValidData;
};
