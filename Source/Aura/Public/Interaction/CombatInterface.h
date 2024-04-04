// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "CombatInterface.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FTaggedMontage
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* Montage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag MontageTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* ImpactSound = nullptr;
};

UINTERFACE(MinimalAPI, BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API ICombatInterface
{
	GENERATED_BODY()

public:
	virtual void Die() = 0;
	virtual int32 GetPlayerLevel() const;

protected:
	// 어빌리티의 블루프린트가 액터에 종속되지않도록 인터페이스에서 구현
	// 모션워핑 컴포넌트를 코드가 아니라 블루프린트에서 직접 추가했으므로 함수내용도 블루프린트에서 구현
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateFacingTarget(const FVector& Target);

	// BlueprintNativeEvent로 블루프린트에서도 구현이 가능하고 C++에서 구현이 가능하다. 그리고 자동적으로 virtual 속성이 붙게됨
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetHitReactMontage() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetCombatSocketLocation(const FGameplayTag& MontageTag) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDead() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetAvatar();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<FTaggedMontage> GetAttackMontages() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UNiagaraSystem* GetBloodEffect() const;
};
