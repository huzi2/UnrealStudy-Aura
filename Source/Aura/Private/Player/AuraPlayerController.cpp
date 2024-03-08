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

	// 인풋 컨피그에서 설정한 입력 액션과 태그를 컨트롤러의 함수와 연결해서, 입력 액션을 하면 해당 입력에 맞는 태그를 얻어올 수 있다.
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

	// 스플라인에 저장된 위치대로 이동
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
		// 마우스 위에 타겟을 클릭했다면 자동 이동은 취소됨
		bTargeting = ThisActor ? true : false;
		bAutoRunning = false;
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!GetAuraAbilitySystemComponent()) return;

	// 마우스 클릭 외 발동
	if (!InputTag.MatchesTagExact(UAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetAuraAbilitySystemComponent())
		{
			GetAuraAbilitySystemComponent()->AbilityInputTagReleased(InputTag);
		}
		return;
	}

	// 땠을 때 능력 발동
	if (GetAuraAbilitySystemComponent())
	{
		GetAuraAbilitySystemComponent()->AbilityInputTagReleased(InputTag);
	}

	// 마우스를 땠을 때 짧게 눌렀으면 목표 위치로 네비게이션 통해서 자동 이동
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

						// 길찾기로 찾은 경로들을 스플라인으로 곡선 형태로 수정
						for (const FVector& PointLoc : NavPath->PathPoints)
						{
							Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);

							//DrawDebugSphere(GetWorld(), PointLoc, 8.f, 8, FColor::Green, false, 5.f);
						}

						// 도착 지점을 네비메시 상 도달할 수 있는 지점으로 설정
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
	// 마우스 클릭 외 발동
	if (!InputTag.MatchesTagExact(UAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetAuraAbilitySystemComponent())
		{
			GetAuraAbilitySystemComponent()->AbilityInputTagHeld(InputTag);
		}
		return;
	}

	// 클릭 중이면 타겟팅일 때 능력 발동. 또는 쉬프트키 누른 상태면 맨 땅에 발동
	if (bTargeting || bShiftKeyDown)
	{
		if (GetAuraAbilitySystemComponent())
		{
			GetAuraAbilitySystemComponent()->AbilityInputTagHeld(InputTag);
		}
	}
	// 마우스 클릭 이동
	else
	{
		if (!GetWorld()) return;

		// 클릭을 누르는 시간을 누적 기록
		FollowTime += GetWorld()->GetDeltaSeconds();

		// 마우스 커서와 지면 충돌 체크
		if (CursorHit.bBlockingHit)
		{
			// 목표 지점은 클릭한 곳
			CachedDestination = CursorHit.ImpactPoint;
		}

		if (APawn* ControllerPawn = GetPawn())
		{
			// 목표 지점으로 방향을 설정
			const FVector WorldDirection = (CachedDestination - ControllerPawn->GetActorLocation()).GetSafeNormal();
			// 해당 방향으로 이동
			ControllerPawn->AddMovementInput(WorldDirection);
		}
	}
}
