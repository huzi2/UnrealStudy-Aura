// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h" 

AAuraPlayerState::AAuraPlayerState()
{
	// GAS 세트를 플레이어 스테이트에서 사용하려면 업데이트 빈도가 높은 것이 좋다.
	// 포트나이트나 라이라 같은 경우에도 높게 설정해서 사용한다고함
	NetUpdateFrequency = 100.f;

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState, Level);
	DOREPLIFETIME(AAuraPlayerState, XP);
	DOREPLIFETIME(AAuraPlayerState, AttributePoints);
	DOREPLIFETIME(AAuraPlayerState, SpellPoints);
}

void AAuraPlayerState::SetLevel(int32 InLevel)
{
	Level = InLevel;

	// bool을 false로 해서 UI가 알림을 띄우지 않도록 한다.(로드했는데 레벨업 알림 뜨는 거 방지)
	OnLevelChangedDelegate.Broadcast(Level, false);
}

void AAuraPlayerState::AddToLevel(int32 InLevel)
{
	Level += InLevel;

	// AddToLevel은 게임 내에서 레벨업할 경우에 사용하기에 UI에 알림
	OnLevelChangedDelegate.Broadcast(Level, true);
}

void AAuraPlayerState::SetXP(int32 InXP)
{
	XP = InXP;

	OnXPChangedDelegate.Broadcast(XP);
}

void AAuraPlayerState::AddToXP(int32 InXP)
{
	XP += InXP;

	OnXPChangedDelegate.Broadcast(XP);
}

void AAuraPlayerState::SetAttributePoints(int32 InAttributePoints)
{
	AttributePoints = InAttributePoints;

	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::AddToAttributePoints(int32 InAttributePoints)
{
	AttributePoints += InAttributePoints;

	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::SetSpellPoints(int32 InSpellPoints)
{
	SpellPoints = InSpellPoints;

	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

void AAuraPlayerState::AddToSpellPoints(int32 InSpellPoints)
{
	SpellPoints += InSpellPoints;

	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelChangedDelegate.Broadcast(Level, true);
}

void AAuraPlayerState::OnRep_XP(int32 OldXP)
{
	OnXPChangedDelegate.Broadcast(XP);
}

void AAuraPlayerState::OnRep_AttributePoints(int32 OldAttributePoints)
{
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::OnRep_SpellPoints(int32 OldSpellPoints)
{
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}
