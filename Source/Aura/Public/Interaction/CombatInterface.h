// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "CombatInterface.generated.h"

class UNiagaraSystem;

/**
 * 태그와 애님 몽타주를 연결
 */
USTRUCT(BlueprintType)
struct FTaggedMontage
{
	GENERATED_BODY()

public:
	// 몽타주 변수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* Montage = nullptr;

	// 공격 몽타주 고유의 태그
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag MontageTag;

	// 공격 몽타주가 사용할 소켓의 태그
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag SocketTag;

	// 해당 공격으로 재생할 사운드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* ImpactSound = nullptr;
};

UINTERFACE(MinimalAPI, BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 전투와 관련된 변수를 얻거나 행동을 하는 인터페이스
 */
class AURA_API ICombatInterface
{
	GENERATED_BODY()

public:
	virtual void Die() = 0;

protected:
	// 캐릭터의 레벨을 얻어온다.
	UFUNCTION(BlueprintNativeEvent)
	int32 GetPlayerLevel() const;

	// 모션워핑 컴포넌트를 통해 객체가 타겟을 향하도록 한다.
	// 어빌리티의 블루프린트가 액터에 종속되지않도록 인터페이스에서 구현
	// 모션워핑 컴포넌트를 코드가 아니라 블루프린트에서 직접 추가했으므로 함수내용도 블루프린트에서 구현
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateFacingTarget(const FVector& Target);

	// 피격 반응 액션을 얻어온다.
	// BlueprintNativeEvent로 블루프린트에서도 구현이 가능하고 C++에서 구현이 가능하다. 그리고 자동적으로 virtual 속성이 붙게됨
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetHitReactMontage() const;

	// 모든 공격 모션을 얻어온다.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<FTaggedMontage> GetAttackMontages() const;

	// 공격 모션에 따라 공격 지점을 얻어온다.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetCombatSocketLocation(const FGameplayTag& MontageTag) const;

	// 죽었는 지 확인
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDead() const;

	// 아바타 액터를 얻어온다.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetAvatar();

	// 피격 이펙트를 얻어온다.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UNiagaraSystem* GetBloodEffect() const;

	// 태그에 따른 몽타주 정보를 얻어온다.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FTaggedMontage GetTaggedMontageByTag(const FGameplayTag& MontageTag) const;

	// 소환수 유닛 수를 얻어온다.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetMinionCount() const;

	// 소환수 유닛 수를 증가
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IncrementMinionCount(int32 Amount);

	// 객체의 직업을 얻어온다.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	ECharacterClass GetCharacterClass() const;
};
