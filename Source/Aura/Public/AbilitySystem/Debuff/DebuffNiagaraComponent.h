// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "GameplayTagContainer.h"
#include "DebuffNiagaraComponent.generated.h"

/**
 * ����� ����Ʈ ����ϴ� ������Ʈ
 */
UCLASS()
class AURA_API UDebuffNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()

private:
	UDebuffNiagaraComponent();

private:
	virtual void BeginPlay() final;

public:
	FORCEINLINE void SetDebuffTag(const FGameplayTag& InDebuffTag) { DebuffTag = InDebuffTag; }

private:
	// ����� �±װ� ����Ǿ��� ��
	void DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	// ���ʰ� �׾��� ��
	UFUNCTION()
	void OnOwnerDeath(AActor* DeadActor);

private:
	UPROPERTY(VisibleAnywhere)
	FGameplayTag DebuffTag;
};
