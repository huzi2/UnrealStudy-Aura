// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Interaction/HighlightInterface.h"
#include "Interaction/EnemyInterface.h"
#include "Input/AuraInputComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "AuraGameplayTags.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "GameFramework/Character.h"
#include "UI/Widget/DamageTextComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Actor/MagicCircle.h"
#include "Components/DecalComponent.h"
#include "Aura/Aura.h"

AAuraPlayerController::AAuraPlayerController()
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

	UpdateMagicCircleLocation();
}

void AAuraPlayerController::ClientShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
{
	if (!IsValid(TargetCharacter)) return;
	if (!DamageTextComponentClass) return;

	// 로컬 플레이어만 해당 데미지를 보면됨
	if (!IsLocalController()) return;

	if (UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass))
	{
		DamageText->RegisterComponent();
		// 어태치로 붙이자마자 블루프린트를 통해서 위젯 애니메이션이 재생될 것
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		// 애니메이션이 재생됬으면 캐릭터에 붙어있을 필요가 없으니 바로 디태치
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
	}
}

void AAuraPlayerController::ShowMagicCircle(UMaterialInstance* DecalMaterial)
{
	if (!GetWorld()) return;
	if (IsValid(MagicCircle)) return;

	MagicCircle = GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass);

	// 새 데칼을 지정했다면 적용
	if (DecalMaterial && MagicCircle && MagicCircle->GetMagicCircleDecal())
	{
		MagicCircle->GetMagicCircleDecal()->SetMaterial(0, DecalMaterial);
	}
}

void AAuraPlayerController::HideMagicCircle()
{
	if (!IsValid(MagicCircle)) return;

	MagicCircle->Destroy();
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
	// Player_Block_InputPressed 태그를 가지고 있는 상태라면 인풋을 막음
	if (GetAuraAbilitySystemComponent() && GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(UAuraGameplayTags::Get().Player_Block_InputPressed))
	{
		return;
	}

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
	// Player_Block_CursorTrace 태그를 가지고 있는 상태라면 커서 트레이스를 막고 모든 하이라이트 제거
	if (GetAuraAbilitySystemComponent() && GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(UAuraGameplayTags::Get().Player_Block_CursorTrace))
	{
		UnHighlightActor(LastActor);
		UnHighlightActor(ThisActor);
		LastActor = nullptr;
		ThisActor = nullptr;
		return;
	}

	// 마법진이 활성화되어있으면 커서에서 플레이어와 몬스터를 무시하는 트레이스를 쏜다(마법진이 캐릭터 위로 올라가는 문제 방지)
	const ECollisionChannel TraceChannel = IsValid(MagicCircle) ? ECC_ExcludePlayers : ECC_Visibility;
	GetHitResultUnderCursor(TraceChannel, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	if (IsValid(CursorHit.GetActor()) && CursorHit.GetActor()->Implements<UHighlightInterface>())
	{
		ThisActor = CursorHit.GetActor();
	}
	else
	{
		ThisActor = nullptr;
	}

	if (LastActor != ThisActor)
	{
		UnHighlightActor(LastActor);
		HighlightActor(ThisActor);
	}
}

void AAuraPlayerController::HighlightActor(AActor* InActor)
{
	if (IsValid(InActor) && InActor->Implements<UHighlightInterface>())
	{
		IHighlightInterface::Execute_HighlightActor(InActor);
	}
}

void AAuraPlayerController::UnHighlightActor(AActor* InActor)
{
	if (IsValid(InActor) && InActor->Implements<UHighlightInterface>())
	{
		IHighlightInterface::Execute_UnHighlightActor(InActor);
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
	// Player_Block_InputPressed 태그를 가지고 있는 상태라면 인풋을 막음
	if (GetAuraAbilitySystemComponent() && GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(UAuraGameplayTags::Get().Player_Block_InputPressed))
	{
		return;
	}

	if (InputTag.MatchesTagExact(UAuraGameplayTags::Get().InputTag_LMB))
	{
		// 타겟팅 상대가 적인지 아닌지 확인
		if (IsValid(ThisActor))
		{
			TargetingStatus = ThisActor->Implements<UEnemyInterface>() ? ETargetingStatus::TargetingEnemy : ETargetingStatus::TargetingNonEnemy;
		}
		else
		{
			TargetingStatus = ETargetingStatus::NotTargeting;
		}

		// 자동 이동은 취소
		bAutoRunning = false;
	}

	// 클릭 발동
	if (GetAuraAbilitySystemComponent())
	{
		GetAuraAbilitySystemComponent()->AbilityInputTagPressed(InputTag);
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	// Player_Block_InputReleased 태그를 가지고 있는 상태라면 인풋을 막음
	if (GetAuraAbilitySystemComponent() && GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(UAuraGameplayTags::Get().Player_Block_InputReleased))
	{
		return;
	}

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
	if (TargetingStatus != ETargetingStatus::TargetingEnemy && !bShiftKeyDown)
	{
		if (FollowTime <= ShortPressThreshold)
		{
			// 하이라이트된 대상이면 이동할 위치를 얻어올 수 있음
			if (IsValid(ThisActor) && ThisActor->Implements<UHighlightInterface>())
			{
				IHighlightInterface::Execute_SetMoveToLocation(ThisActor, CachedDestination);
			}
			else if (GetAuraAbilitySystemComponent() && !GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(UAuraGameplayTags::Get().Player_Block_InputPressed))
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickNiagaraSystem, CachedDestination);
			}

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
						if (NavPath->PathPoints.Num() > 0)
						{
							CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
							bAutoRunning = true;
						}
					}
				}
			}
		}
		FollowTime = 0.f;
		TargetingStatus = ETargetingStatus::NotTargeting;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	// Player_Block_InputHeld 태그를 가지고 있는 상태라면 인풋을 막음
	if (GetAuraAbilitySystemComponent() && GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(UAuraGameplayTags::Get().Player_Block_InputHeld))
	{
		return;
	}

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
	if (TargetingStatus == ETargetingStatus::TargetingEnemy || bShiftKeyDown)
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

void AAuraPlayerController::UpdateMagicCircleLocation()
{
	if (!IsValid(MagicCircle)) return;

	MagicCircle->SetActorLocation(CursorHit.ImpactPoint);
}
