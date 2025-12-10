// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "GameplayTagContainer.h"
#include "DebuffNiagaraComponent.generated.h"

/**
 * 디버프 이펙트 출력하는 컴포넌트
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
	// 디버프 태그가 변경되었을 때
	void DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	// 오너가 죽었을 때
	UFUNCTION()
	void OnOwnerDeath(AActor* DeadActor);

private:
	UPROPERTY(VisibleAnywhere)
	FGameplayTag DebuffTag;
};
