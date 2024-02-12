// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/AuraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

AAuraCharacter::AAuraCharacter()
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0, 400.0, 0.0);
		GetCharacterMovement()->bConstrainToPlane = true;
		GetCharacterMovement()->bSnapToPlaneAtStart = true;
	}

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 서버에서 어빌리티 시스템 컴포넌트 초기화
	InitAbilityActorInfo();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// 클라에서 어빌리티 시스템 컴포넌트 초기화
	InitAbilityActorInfo();
}

void AAuraCharacter::InitAbilityActorInfo()
{
	// 플레이어의 오너 액터는 플레이어 스테이트, 아바타 액터는 캐릭터가 된다.
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	if (AuraPlayerState->GetAbilitySystemComponent())
	{
		AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
		AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
		AttributeSet = AuraPlayerState->GetAttributeSet();
	}
}
