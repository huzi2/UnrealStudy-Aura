// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Interaction/EnemyInterface.h"
#include "Input/AuraInputComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "AuraGameplayTags.h"

AAuraPlayerController::AAuraPlayerController()
	: AutoRunAcceptanceRadius(50.f)
	, CachedDestination(FVector::ZeroVector)
	, FollowTime(0.f)
	, ShortPressThreshold(0.5f)
	, bAutoRunning(false)
	, bTargeting(false)
{
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(AuraContext);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);

	// ��ǲ ���Ǳ׿��� ������ �Է� �׼ǰ� �±׸� ��Ʈ�ѷ��� �Լ��� �����ؼ�, �Է� �׼��� �ϸ� �ش� �Է¿� �´� �±׸� ���� �� �ִ�.
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetAuraAbilitySystemComponent()
{
	if (!AuraAbilitySystemComponent)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent;
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.0, Rotation.Yaw, 0.0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControllPawn = GetPawn<APawn>())
	{
		ControllPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControllPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	if (!LastActor)
	{
		// ���� Ÿ���� ���� ���¿��� �� Ÿ������ ���������Ƿ� ���̶���Ʈ
		if (ThisActor)
		{
			ThisActor->HighlightActor();
		}
	}
	else
	{
		// ���� Ÿ���� �ִ� ���¿��� �� ������ ���������Ƿ� �����̶���Ʈ
		if (!ThisActor)
		{
			LastActor->UnHighlightActor();
		}
		else
		{
			// ���� Ÿ���� �ִ� ���¿��� �ٸ� Ÿ������ ���������Ƿ� ���̶���Ʈ�� ����
			if (LastActor != ThisActor)
			{
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();
			}
		}
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(UAuraGameplayTags::Get().InputTag_LMB))
	{
		// ���콺 ���� Ÿ���� Ŭ���ߴٸ� �ڵ� �̵��� ��ҵ�
		bTargeting = ThisActor ? true : false;
		bAutoRunning = false;
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!GetAuraAbilitySystemComponent()) return;

	GetAuraAbilitySystemComponent()->AbilityInputTagReleased(InputTag);
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	// ���콺 Ŭ�� �� �ߵ�
	if (!InputTag.MatchesTagExact(UAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetAuraAbilitySystemComponent())
		{
			GetAuraAbilitySystemComponent()->AbilityInputTagHeld(InputTag);
		}
		return;
	}

	// Ŭ�� ���̸� Ÿ������ �� �ɷ� �ߵ�, �ƴϸ� �̵�
	if (bTargeting)
	{
		if (GetAuraAbilitySystemComponent())
		{
			GetAuraAbilitySystemComponent()->AbilityInputTagHeld(InputTag);
		}
	}
	// ���콺 Ŭ�� �̵�
	else
	{
		if (!GetWorld()) return;

		// Ŭ���� ������ �ð��� ���� ���
		FollowTime += GetWorld()->GetDeltaSeconds();

		// ���콺 Ŀ���� ���� �浹 üũ
		FHitResult Hit;
		if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
		{
			// ��ǥ ������ Ŭ���� ��
			CachedDestination = Hit.ImpactPoint;
		}

		if (APawn* ControllerPawn = GetPawn())
		{
			// ��ǥ �������� ������ ����
			const FVector WorldDirection = (CachedDestination - ControllerPawn->GetActorLocation()).GetSafeNormal();
			// �ش� �������� �̵�
			ControllerPawn->AddMovementInput(WorldDirection);
		}
	}
}
