// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderMouse.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle);

/**
 * 
 */
UCLASS()
class AURA_API UTargetDataUnderMouse : public UAbilityTask
{
	GENERATED_BODY()
	
private:
	virtual void Activate() final;

private:
	// �������Ʈ���� TargetDataUnderMouse�� �̸����� ȣ��ǰ�, OwningAbility ���� ����� self�� �⺻���� ����. BlueprintInternalUseOnly�� �ش� �Լ��� �������Ʈ ���ο����� ���Ǿ���Ѵٴ� ��
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "TargetDataUnderMouse", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UTargetDataUnderMouse* CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility);

private:
	void SendMouseCursorData();
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);

private:
	// �������Ʈ���� CreateTargetDataUnderMouse() �Լ��� ������ �ڵ� ����ȴ�.
	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignature ValidData;
};
