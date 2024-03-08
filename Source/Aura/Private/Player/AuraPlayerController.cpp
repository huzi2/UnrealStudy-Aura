// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Interaction/EnemyInterface.h"
#include "Input/AuraInputComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "AuraGameplayTags.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

AAuraPlayerController::AAuraPlayerController()
	: AutoRunAcceptanceRadius(50.0)
	, CachedDestination(FVector::ZeroVector)
	, FollowTime(0.f)
	, ShortPressThreshold(0.5f)
	, bAutoRunning(false)
	, bTargeting(false)
	, bShiftKeyDown(false)
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
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &ThisClass::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &ThisClass::ShiftReleased);

	// ��ǲ ���Ǳ׿��� ������ �Է� �׼ǰ� �±׸� ��Ʈ�ѷ��� �Լ��� �����ؼ�, �Է� �׼��� �ϸ� �ش� �Է¿� �´� �±׸� ���� �� �ִ�.
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();

	AutoRun();
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
	GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	if (LastActor != ThisActor)
	{
		if (LastActor) LastActor->UnHighlightActor();
		if (ThisActor)ThisActor->HighlightActor();
	}
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return;

	// ���ö��ο� ����� ��ġ��� �̵�
	if (APawn* ControllerPawn = GetPawn())
	{
		if (Spline)
		{
			const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControllerPawn->GetActorLocation(), ESplineCoordinateSpace::World);
			const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);

			ControllerPawn->AddMovementInput(Direction);

			const double DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
			if (DistanceToDestination <= AutoRunAcceptanceRadius)
			{
				bAutoRunning = false;
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

	// ���콺 Ŭ�� �� �ߵ�
	if (!InputTag.MatchesTagExact(UAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetAuraAbilitySystemComponent())
		{
			GetAuraAbilitySystemComponent()->AbilityInputTagReleased(InputTag);
		}
		return;
	}

	// ���� �� �ɷ� �ߵ�
	if (GetAuraAbilitySystemComponent())
	{
		GetAuraAbilitySystemComponent()->AbilityInputTagReleased(InputTag);
	}

	// ���콺�� ���� �� ª�� �������� ��ǥ ��ġ�� �׺���̼� ���ؼ� �ڵ� �̵�
	if (!bTargeting && !bShiftKeyDown)
	{
		if (FollowTime <= ShortPressThreshold)
		{
			if (const APawn* ControllPawn = GetPawn())
			{
				if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControllPawn->GetActorLocation(), CachedDestination))
				{
					if (Spline)
					{
						Spline->ClearSplinePoints();

						// ��ã��� ã�� ��ε��� ���ö������� � ���·� ����
						for (const FVector& PointLoc : NavPath->PathPoints)
						{
							Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);

							//DrawDebugSphere(GetWorld(), PointLoc, 8.f, 8, FColor::Green, false, 5.f);
						}

						// ���� ������ �׺�޽� �� ������ �� �ִ� �������� ����
						CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];

						bAutoRunning = true;
					}
				}
			}
		}
		FollowTime = 0.f;
		bTargeting = false;
	}
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

	// Ŭ�� ���̸� Ÿ������ �� �ɷ� �ߵ�. �Ǵ� ����ƮŰ ���� ���¸� �� ���� �ߵ�
	if (bTargeting || bShiftKeyDown)
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
		if (CursorHit.bBlockingHit)
		{
			// ��ǥ ������ Ŭ���� ��
			CachedDestination = CursorHit.ImpactPoint;
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
