// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/AuraEnemy.h"
#include "Aura/Aura.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"
#include "Components/WidgetComponent.h"
#include "UI/Widget/AuraUserWidget.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AuraGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/AuraAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECollisionResponse::ECR_Overlap);

	// 타겟으로의 회전을 무브먼트 컴포넌트가 해서 좀 더 자연스럽게 타겟쪽으로 회전하도록 함
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>(TEXT("AttributeSet"));

	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBar->SetupAttachment(GetRootComponent());
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	}

	check(AbilitySystemComponent);

	InitAbilityActorInfo();

	// 기본 어빌리티는 적캐릭터의 직업에 맞춰서 가져온다. 그런 정보는 게임모드에 저장되어있으므로 서버에서만 수행하도록 한다.
	if (HasAuthority())
	{
		// 기본 어빌리티들 부여
		UAuraAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent, CharacterClass);
	}

	// 위젯 컨트롤러를 자신으로 설정한다. 위젯 컨트롤러는 단순한 UObject이므로 가능함
	if (UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		AuraUserWidget->SetWidgetController(this);
	}

	// 체력 어트리뷰트 세트가 변경될 때 UI에 알리기위해 델리게이트와 바인드함
	// 프로그래스바 위젯의 블루프린트 그래프에서 해당 델리게이트의 이벤트에 대한 행동(체력바 수정)을 한다.
	if (const UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);

		// Effects.HitReact 태그를 얻었을 때 피격 모션을 재생함하는 함수를 바인드
		AbilitySystemComponent->RegisterGameplayTagEvent(UAuraGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::HitReactTagChanged);

		// 초기값 설정
		OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
		OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	}
}

void AAuraEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// AI는 서버에서만 사용
	if (!HasAuthority()) return;

	if (AuraAIController = Cast<AAuraAIController>(NewController))
	{
		if (AuraAIController->GetBlackboardComponent() && BehaviorTree)
		{
			AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
			AuraAIController->RunBehaviorTree(BehaviorTree);
			
			// 블랙보드 초기값 설정
			AuraAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("HitReacting"), false);
			AuraAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("RangedAttacker"), CharacterClass != ECharacterClass::Warrior);
		}
	}
}

void AAuraEnemy::InitAbilityActorInfo()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		
		// 기절 태그에 대한 콜백 함수 설정
		AbilitySystemComponent->RegisterGameplayTagEvent(UAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::StunTagChanged);
	}
	
	if (UAuraAbilitySystemComponent* AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		OnAbilitySystemComponentRegisteredDelegate.Broadcast(AuraAbilitySystemComponent);
		AuraAbilitySystemComponent->AbilityActorInfoSet();
	}

	// 적캐릭터의 직업에 맞춰서 초기화. 그런 정보는 게임모드에 저장되어있으므로 서버에서만 수행하도록 한다.
	if (HasAuthority())
	{
		// 게임플레이 이펙트를 통한 능력치 초기화
		InitializeDefaultAttributes();
	}
}

void AAuraEnemy::InitializeDefaultAttributes() const
{
	UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, static_cast<float>(Level), AbilitySystemComponent);
}

void AAuraEnemy::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::StunTagChanged(CallbackTag, NewCount);

	// 비헤이비어 트리에 적용
	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("Stunned"), bIsStunned);
	}
}

void AAuraEnemy::Die(const FVector& DeathImpulse)
{
	Super::Die(DeathImpulse);

	SetLifeSpan(5.f);

	// 블랙보드에 죽었다고 체크
	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("Dead"), true);
	}
}

int32 AAuraEnemy::GetPlayerLevel_Implementation() const
{
	return Level;
}

void AAuraEnemy::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AAuraEnemy::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}

void AAuraEnemy::HighlightActor_Implementation()
{
	if (!GetMesh()) return;

	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);

	if (Weapon)
	{
		Weapon->SetRenderCustomDepth(true);
		Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	}
}

void AAuraEnemy::UnHighlightActor_Implementation()
{
	if (!GetMesh()) return;

	GetMesh()->SetRenderCustomDepth(false);

	if (Weapon)
	{
		Weapon->SetRenderCustomDepth(false);
	}
}

void AAuraEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	// 새로운 태그가 들어올 때만
	bHitReacting = NewCount > 0;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
	}

	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("HitReacting"), bHitReacting);
	}
}
