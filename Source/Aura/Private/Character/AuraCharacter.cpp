// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/AuraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Player/AuraPlayerController.h"
#include "UI/HUD/AuraHUD.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "NiagaraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Game/AuraGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "AbilitySystem/AuraAttributeSet.h"

AAuraCharacter::AAuraCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>("TopDownCameraComponent");
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;

	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LevelUpNiagaraComponent"));
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->bAutoActivate = false;

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

	CharacterClass = ECharacterClass::Elementalist;
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 서버에서 어빌리티 시스템 컴포넌트 초기화
	InitAbilityActorInfo();

	// 게임 시작부터 주어지는 어빌리티 
	AddCharacterAbilities();
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

	if (UAuraAbilitySystemComponent* AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent()))
	{
		AuraAbilitySystemComponent->AbilityActorInfoSet();

		// 어빌리티 시스템 컴포넌트가 연결되었다는 델리게이트 호출
		OnAbilitySystemComponentRegisteredDelegate.Broadcast(AuraAbilitySystemComponent);

		// 기절 태그에 대한 콜백 함수 설정
		AuraAbilitySystemComponent->RegisterGameplayTagEvent(UAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::StunTagChanged);
	}

	if (AuraPlayerState->GetAbilitySystemComponent())
	{
		AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
		AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();

		AttributeSet = AuraPlayerState->GetAttributeSet();

		// 위젯 컨트롤러 초기화. 아래 4가지 변수를 모두 얻을 수 있는 곳이므로 여기서 초기화함
		if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
		{
			if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
			{
				AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
			}
		}
	}

	// 스탯 어트리뷰트 게임플레이 이펙트로 초기화
	InitializeDefaultAttributes();
}

void AAuraCharacter::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

void AAuraCharacter::OnRep_Burned()
{
	// 클라에도 불타는 이펙트 적용
	if (bIsBurned)
	{
		if (BurnDebuffComponent)
		{
			BurnDebuffComponent->Activate();
		}
	}
	else
	{
		if (BurnDebuffComponent)
		{
			BurnDebuffComponent->Deactivate();
		}
	}
}

void AAuraCharacter::OnRep_Stunned()
{
	// 스턴 태그가 들어왔을 때 클라에서도 입력 막는 태그를 적용. 이펙트도 적용
	if (UAuraAbilitySystemComponent* AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		const UAuraGameplayTags& GameplayTags = UAuraGameplayTags::Get();
		FGameplayTagContainer BlockedTags;
		BlockedTags.AddTag(GameplayTags.Player_Block_CursorTrace);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputHeld);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputPressed);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputReleased);

		if (bIsStunned)
		{
			AuraAbilitySystemComponent->AddLooseGameplayTags(BlockedTags);

			if (StunDebuffComponent)
			{
				StunDebuffComponent->Activate();
			}
		}
		else
		{
			AuraAbilitySystemComponent->RemoveLooseGameplayTags(BlockedTags);

			if (StunDebuffComponent)
			{
				StunDebuffComponent->Deactivate();
			}
		}
	}
}

int32 AAuraCharacter::GetPlayerLevel_Implementation() const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->GetPlayerLevel();
}

int32 AAuraCharacter::FindLevelForXP_Implementation(int32 InXP) const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	if (!AuraPlayerState->GetLevelUpInfo()) return 0;

	return AuraPlayerState->GetLevelUpInfo()->FindLevelForXP(InXP);
}

int32 AAuraCharacter::GetXP_Implementation() const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->GetXP();
}

void AAuraCharacter::AddToXP_Implementation(int32 InXP)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	AuraPlayerState->AddToXP(InXP);
}

void AAuraCharacter::LevelUp_Implementation()
{
	MulticastLevelUpParticles();
}

void AAuraCharacter::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	AuraPlayerState->AddToLevel(InPlayerLevel);

	// 레벨업하면서 활성화시킬 수 있는 스킬 확인해서 활성화
	if (UAuraAbilitySystemComponent* AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent()))
	{
		AuraAbilitySystemComponent->UpdateAbilityStatuses(AuraPlayerState->GetPlayerLevel());
	}
}

int32 AAuraCharacter::GetAttributePoints_Implementation() const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->GetAttributePoints();
}

int32 AAuraCharacter::GetAttributePointReward_Implementation(int32 Level) const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	if (!AuraPlayerState->GetLevelUpInfo()) return 0;
	if (!AuraPlayerState->GetLevelUpInfo()->GetLevelUpInformation().IsValidIndex(Level)) return 0;

	return AuraPlayerState->GetLevelUpInfo()->GetLevelUpInformation()[Level].AttributePointAward;
}

void AAuraCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	AuraPlayerState->AddToAttributePoints(InAttributePoints);
}

int32 AAuraCharacter::GetSpellPoints_Implementation() const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->GetSpellPoints();
}

int32 AAuraCharacter::GetSpellPointReward_Implementation(int32 Level) const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	if (!AuraPlayerState->GetLevelUpInfo()) return 0;
	if (!AuraPlayerState->GetLevelUpInfo()->GetLevelUpInformation().IsValidIndex(Level)) return 0;

	return AuraPlayerState->GetLevelUpInfo()->GetLevelUpInformation()[Level].SpellPointAward;
}

void AAuraCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	AuraPlayerState->AddToSpellPoints(InSpellPoints);
}

void AAuraCharacter::ShowMagicCircle_Implementation(UMaterialInstance* DecalMaterial)
{
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		AuraPlayerController->ShowMagicCircle(DecalMaterial);
		AuraPlayerController->bShowMouseCursor = false;
	}
}

void AAuraCharacter::HideMagicCircle_Implementation()
{
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		AuraPlayerController->HideMagicCircle();
		AuraPlayerController->bShowMouseCursor = true;
	}
}

void AAuraCharacter::SaveProgress_Implementation(const FName& CheckpointTag)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (!AuraGameMode) return;

	ULoadScreenSaveGame* SaveData = AuraGameMode->RetrieveInGameSaveData();
	if (!SaveData) return;

	// 세이브 데이터에 플레이어 정보 기입
	SaveData->PlayerStartTag = CheckpointTag;

	if (AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState()))
	{
		SaveData->PlayerLevel = AuraPlayerState->GetPlayerLevel();
		SaveData->XP = AuraPlayerState->GetXP();
		SaveData->AttributePoints = AuraPlayerState->GetAttributePoints();
		SaveData->SpellPoints = AuraPlayerState->GetSpellPoints();
	}

	SaveData->Strength = UAuraAttributeSet::GetStrengthAttribute().GetNumericValue(GetAttributeSet());
	SaveData->Intelligence = UAuraAttributeSet::GetIntelligenceAttribute().GetNumericValue(GetAttributeSet());
	SaveData->Resilience = UAuraAttributeSet::GetResilienceAttribute().GetNumericValue(GetAttributeSet());
	SaveData->Vigor = UAuraAttributeSet::GetVigorAttribute().GetNumericValue(GetAttributeSet());

	// 최신화한 세이브 데이터 저장
	AuraGameMode->SaveInGameProgressData(SaveData);
}

void AAuraCharacter::MulticastLevelUpParticles_Implementation() const
{
	// 레벨 업 이펙트 재생
	if (IsValid(LevelUpNiagaraComponent) && IsValid(TopDownCameraComponent))
	{
		// 레벨 업 이펙트가 카메라 쪽으로 보여지도록 방향 수정
		const FVector CameraLocation = TopDownCameraComponent->GetComponentLocation();
		const FVector NiagaraSystemLocation = LevelUpNiagaraComponent->GetComponentLocation();
		const FRotator ToCameraRotation = (CameraLocation - NiagaraSystemLocation).Rotation();
		LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);

		LevelUpNiagaraComponent->Activate(true);
	}
}
