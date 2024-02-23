// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/AuraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Player/AuraPlayerController.h"
#include "UI/HUD/AuraHUD.h"

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

	// �������� �����Ƽ �ý��� ������Ʈ �ʱ�ȭ
	InitAbilityActorInfo();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Ŭ�󿡼� �����Ƽ �ý��� ������Ʈ �ʱ�ȭ
	InitAbilityActorInfo();
}

void AAuraCharacter::InitAbilityActorInfo()
{
	// �÷��̾��� ���� ���ʹ� �÷��̾� ������Ʈ, �ƹ�Ÿ ���ʹ� ĳ���Ͱ� �ȴ�.
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	if (UAuraAbilitySystemComponent* AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent()))
	{
		AuraAbilitySystemComponent->AbilityActorInfoSet();
	}

	if (AuraPlayerState->GetAbilitySystemComponent())
	{
		AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
		AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
		AttributeSet = AuraPlayerState->GetAttributeSet();

		// ���� ��Ʈ�ѷ� �ʱ�ȭ. �Ʒ� 4���� ������ ��� ���� �� �ִ� ���̹Ƿ� ���⼭ �ʱ�ȭ��
		if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
		{
			if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
			{
				AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
			}
		}
	}

	// �⺻ ���� ��Ʈ����Ʈ �ʱ�ȭ
	InitializePrimaryAttributes();
}
