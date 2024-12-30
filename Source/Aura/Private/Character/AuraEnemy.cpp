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

	// Ÿ�������� ȸ���� �����Ʈ ������Ʈ�� �ؼ� �� �� �ڿ������� Ÿ�������� ȸ���ϵ��� ��
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

	// �⺻ �����Ƽ�� ��ĳ������ ������ ���缭 �����´�. �׷� ������ ���Ӹ�忡 ����Ǿ������Ƿ� ���������� �����ϵ��� �Ѵ�.
	if (HasAuthority())
	{
		// �⺻ �����Ƽ�� �ο�
		UAuraAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent, CharacterClass);
	}

	// ���� ��Ʈ�ѷ��� �ڽ����� �����Ѵ�. ���� ��Ʈ�ѷ��� �ܼ��� UObject�̹Ƿ� ������
	if (UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		AuraUserWidget->SetWidgetController(this);
	}

	// ü�� ��Ʈ����Ʈ ��Ʈ�� ����� �� UI�� �˸������� ��������Ʈ�� ���ε���
	// ���α׷����� ������ �������Ʈ �׷������� �ش� ��������Ʈ�� �̺�Ʈ�� ���� �ൿ(ü�¹� ����)�� �Ѵ�.
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

		// Effects.HitReact �±׸� ����� �� �ǰ� ����� ������ϴ� �Լ��� ���ε�
		AbilitySystemComponent->RegisterGameplayTagEvent(UAuraGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::HitReactTagChanged);

		// �ʱⰪ ����
		OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
		OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	}
}

void AAuraEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// AI�� ���������� ���
	if (!HasAuthority()) return;

	if (AuraAIController = Cast<AAuraAIController>(NewController))
	{
		if (AuraAIController->GetBlackboardComponent() && BehaviorTree)
		{
			AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
			AuraAIController->RunBehaviorTree(BehaviorTree);
			
			// ������ �ʱⰪ ����
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
		
		// ���� �±׿� ���� �ݹ� �Լ� ����
		AbilitySystemComponent->RegisterGameplayTagEvent(UAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::StunTagChanged);
	}
	
	if (UAuraAbilitySystemComponent* AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		OnAbilitySystemComponentRegisteredDelegate.Broadcast(AuraAbilitySystemComponent);
		AuraAbilitySystemComponent->AbilityActorInfoSet();
	}

	// ��ĳ������ ������ ���缭 �ʱ�ȭ. �׷� ������ ���Ӹ�忡 ����Ǿ������Ƿ� ���������� �����ϵ��� �Ѵ�.
	if (HasAuthority())
	{
		// �����÷��� ����Ʈ�� ���� �ɷ�ġ �ʱ�ȭ
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

	// �����̺�� Ʈ���� ����
	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("Stunned"), bIsStunned);
	}
}

void AAuraEnemy::Die(const FVector& DeathImpulse)
{
	Super::Die(DeathImpulse);

	SetLifeSpan(5.f);

	// �����忡 �׾��ٰ� üũ
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
	// ���ο� �±װ� ���� ����
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
