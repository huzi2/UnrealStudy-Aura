// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AuraPlayerState.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class ULevelUpInfo;

// 플레이어 스테이트의 변수가 변경되었을 때 호출할 델리게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, int32);

/**
 * 레벨과 경험치를 관리하는 플레이어 스테이트 클래스
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

private:
	AAuraPlayerState();

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const final;

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const final;

public:
	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
	FORCEINLINE int32 GetXP() const { return XP; }
	FORCEINLINE int32 GetAttributePoints() const { return AttributePoints; }
	FORCEINLINE int32 GetSpellPoints() const { return SpellPoints; }
	FORCEINLINE ULevelUpInfo* GetLevelUpInfo() const { return LevelUpInfo; }
	FORCEINLINE UAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
	void SetLevel(int32 InLevel);
	void AddToLevel(int32 InLevel);

	void SetXP(int32 InXP);
	void AddToXP(int32 InXP);

	//void SetAttributePoints(int32 InAttributePoints);
	void AddToAttributePoints(int32 InAttributePoints);

	//void SetSpellPoints(int32 InSpellPoints);
	void AddToSpellPoints(int32 InSpellPoints);

public:
	// 레벨이 변경되었을 때 호출할 델리게이트
	FOnPlayerStateChanged OnLevelChangedDelegate;
	// XP가 변경되었을 때 호출할 델리게이트
	FOnPlayerStateChanged OnXPChangedDelegate;
	// 능력치 포인트가 변경되었을 때 호출할 델리게이트
	FOnPlayerStateChanged OnAttributePointsChangedDelegate;
	// 스킬 포인트가 변경되었을 때 호출할 델리게이트
	FOnPlayerStateChanged OnSpellPointsChangedDelegate;

private:
	// 레벨은 복잡한 어트리뷰트 기능이 필요없고, float와 어울리지 않아서 어트리뷰트로 설정하지 않고 직접 사용한다.
	// 레벨
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
	int32 Level = 1;
	UFUNCTION()
	void OnRep_Level(int32 OldLevel);
	// 경험치
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_XP)
	int32 XP = 1;
	UFUNCTION()
	void OnRep_XP(int32 OldXP);
	// 능력치 포인트
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_AttributePoints)
	int32 AttributePoints = 0;
	UFUNCTION()
	void OnRep_AttributePoints(int32 OldAttributePoints);
	// 스펠 포인트
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SpellPoints)
	int32 SpellPoints = 0;
	UFUNCTION()
	void OnRep_SpellPoints(int32 OldSpellPoints);

	// 레벨 업 정보
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;

	// 플레이어는 PlayerState에서 어빌리티와 어트리뷰트를 가짐
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	// 참조 변수
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
};
