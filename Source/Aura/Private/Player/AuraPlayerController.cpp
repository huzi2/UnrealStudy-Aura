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

	// ��ǲ ���Ǳ׿��� ������ �Է� �׼ǰ� �±׸� ��Ʈ�ѷ��� �Լ��� �����ؼ�, �Է� �׼��� �ϸ� �ش� �Է¿� �´� �±׸� ���� �� �ִ�.
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

	// ���� �÷��̾ �ش� �������� �����
	if (!IsLocalController()) return;

	if (UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass))
	{
		DamageText->RegisterComponent();
		// ����ġ�� �����ڸ��� �������Ʈ�� ���ؼ� ���� �ִϸ��̼��� ����� ��
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		// �ִϸ��̼��� ��������� ĳ���Ϳ� �پ����� �ʿ䰡 ������ �ٷ� ����ġ
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
	}
}

void AAuraPlayerController::ShowMagicCircle(UMaterialInstance* DecalMaterial)
{
	if (!GetWorld()) return;
	if (IsValid(MagicCircle)) return;

	MagicCircle = GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass);

	// �� ��Į�� �����ߴٸ� ����
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
	// Player_Block_InputPressed �±׸� ������ �ִ� ���¶�� ��ǲ�� ����
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
	// Player_Block_CursorTrace �±׸� ������ �ִ� ���¶�� Ŀ�� Ʈ���̽��� ���� ��� ���̶���Ʈ ����
	if (GetAuraAbilitySystemComponent() && GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(UAuraGameplayTags::Get().Player_Block_CursorTrace))
	{
		UnHighlightActor(LastActor);
		UnHighlightActor(ThisActor);
		LastActor = nullptr;
		ThisActor = nullptr;
		return;
	}

	// �������� Ȱ��ȭ�Ǿ������� Ŀ������ �÷��̾�� ���͸� �����ϴ� Ʈ���̽��� ���(�������� ĳ���� ���� �ö󰡴� ���� ����)
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
	// Player_Block_InputPressed �±׸� ������ �ִ� ���¶�� ��ǲ�� ����
	if (GetAuraAbilitySystemComponent() && GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(UAuraGameplayTags::Get().Player_Block_InputPressed))
	{
		return;
	}

	if (InputTag.MatchesTagExact(UAuraGameplayTags::Get().InputTag_LMB))
	{
		// Ÿ���� ��밡 ������ �ƴ��� Ȯ��
		if (IsValid(ThisActor))
		{
			TargetingStatus = ThisActor->Implements<UEnemyInterface>() ? ETargetingStatus::TargetingEnemy : ETargetingStatus::TargetingNonEnemy;
		}
		else
		{
			TargetingStatus = ETargetingStatus::NotTargeting;
		}

		// �ڵ� �̵��� ���
		bAutoRunning = false;
	}

	// Ŭ�� �ߵ�
	if (GetAuraAbilitySystemComponent())
	{
		GetAuraAbilitySystemComponent()->AbilityInputTagPressed(InputTag);
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	// Player_Block_InputReleased �±׸� ������ �ִ� ���¶�� ��ǲ�� ����
	if (GetAuraAbilitySystemComponent() && GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(UAuraGameplayTags::Get().Player_Block_InputReleased))
	{
		return;
	}

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
	if (TargetingStatus != ETargetingStatus::TargetingEnemy && !bShiftKeyDown)
	{
		if (FollowTime <= ShortPressThreshold)
		{
			// ���̶���Ʈ�� ����̸� �̵��� ��ġ�� ���� �� ����
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

						// ��ã��� ã�� ��ε��� ���ö������� � ���·� ����
						for (const FVector& PointLoc : NavPath->PathPoints)
						{
							Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);

							//DrawDebugSphere(GetWorld(), PointLoc, 8.f, 8, FColor::Green, false, 5.f);
						}

						// ���� ������ �׺�޽� �� ������ �� �ִ� �������� ����
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
	// Player_Block_InputHeld �±׸� ������ �ִ� ���¶�� ��ǲ�� ����
	if (GetAuraAbilitySystemComponent() && GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(UAuraGameplayTags::Get().Player_Block_InputHeld))
	{
		return;
	}

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
	if (TargetingStatus == ETargetingStatus::TargetingEnemy || bShiftKeyDown)
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

void AAuraPlayerController::UpdateMagicCircleLocation()
{
	if (!IsValid(MagicCircle)) return;

	MagicCircle->SetActorLocation(CursorHit.ImpactPoint);
}
