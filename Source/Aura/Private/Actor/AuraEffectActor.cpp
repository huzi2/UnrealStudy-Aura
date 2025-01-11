// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/AuraEffectActor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetMathLibrary.h"

AAuraEffectActor::AAuraEffectActor()
	: bDestroyOnEffectApplication(false)
	, bApplyEffectsToEnemies(false)
	, InstantEffectApplicationPolicy(EEffectApplicationPolicy::DoNotApply)
	, DurationEffectApplicationPolicy(EEffectApplicationPolicy::DoNotApply)
	, InfiniteEffectApplicationPolicy(EEffectApplicationPolicy::DoNotApply)
	, InfiniteEffectRemovalPolicy(EEffectRemovalPolicy::RemoveOnEndOverlap)
	, ActorLevel(1.f)
{
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot")));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();

	InitialLocation = GetActorLocation();
	CalculatedLocation = InitialLocation;
	CalculatedRotation = GetActorRotation();
}

void AAuraEffectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	const float SinePeriod = 2.f * PI / SinePeriodConstant;
	// ���Ʒ� �ֱ⸶�� �ʱ�ȭ
	if (RunningTime > SinePeriod)
	{
		RunningTime = 0.f;
	}

	ItemMovement(DeltaTime);
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	if (!TargetActor) return;

	// �����Դ� ������� �ʴ� ����Ʈ�� ����
	if (TargetActor->ActorHasTag(TEXT("Enemy")) && !bApplyEffectsToEnemies) return;

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC) return;
	check(GameplayEffectClass);

	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	// ����Ʈ�� ����Ų ��ü
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);
	// ����Ʈ �ߵ�
	FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	// ����Ʈ�� ������ ��� ���߿� �����ϱ����� FActiveGameplayEffectHandle�� UAbilitySystemComponent�� �����صд�.
	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (bIsInfinite && InfiniteEffectRemovalPolicy != EEffectRemovalPolicy::DoNotRemove)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}

	// ����Ʈ ������ ������ �ƴϸ鼭 ���� �� ���ŵǾ��Ѵٸ� ������
	if (bDestroyOnEffectApplication && !bIsInfinite)
	{
		Destroy();
	}
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}

	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}

	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}

	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}

	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}

	// ���� ����Ʈ�� ����
	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC)) return;

		TArray<FActiveGameplayEffectHandle> HandlesToRemove;

		for (const TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*>& HandPair : ActiveEffectHandles)
		{
			if (TargetASC == HandPair.Value)
			{
				// ����Ʈ�� ����. �� ������ ������ ����. ����Ʈ�� ���θ� ��� ������ ����
				TargetASC->RemoveActiveGameplayEffect(HandPair.Key, 1);
				// ���� ������ ���ŵ� ����Ʈ���� TMap���� �����ϱ� ���� ���� ����
				HandlesToRemove.Add(HandPair.Key);
			}
		}

		// TMap ����
		for (const FActiveGameplayEffectHandle& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}
}

void AAuraEffectActor::StartRotation()
{
	bRotates = true;
	CalculatedRotation = GetActorRotation();
}

void AAuraEffectActor::StartSinusoidalMovement()
{
	bSinusoidalMovement = true;
	InitialLocation = GetActorLocation();
	CalculatedLocation = InitialLocation;
}

void AAuraEffectActor::ItemMovement(float DeltaTime)
{
	// ȸ��
	if (bRotates)
	{
		const FRotator DeltaRotation(0.f, DeltaTime * RotationRate, 0.f);
		CalculatedRotation = UKismetMathLibrary::ComposeRotators(CalculatedRotation, DeltaRotation);
	}

	// ���Ʒ� ���� �ĵ� ������
	if (bSinusoidalMovement)
	{
		const float Sine = SineAmplitude * FMath::Sin(RunningTime * SinePeriodConstant);
		CalculatedLocation = InitialLocation + FVector(0.f, 0.f, Sine);
	}
}
