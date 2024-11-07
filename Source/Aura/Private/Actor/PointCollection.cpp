// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/PointCollection.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

APointCollection::APointCollection()
{
	PrimaryActorTick.bCanEverTick = false;

	Pt_0 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_0"));
	SetRootComponent(Pt_0);
	ImmutablePts.Add(Pt_0);

	Pt_1 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_1"));
	Pt_1->SetupAttachment(GetRootComponent());
	ImmutablePts.Add(Pt_1);

	Pt_2 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_2"));
	Pt_2->SetupAttachment(GetRootComponent());
	ImmutablePts.Add(Pt_2);

	Pt_3 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_3"));
	Pt_3->SetupAttachment(GetRootComponent());
	ImmutablePts.Add(Pt_3);

	Pt_4 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_4"));
	Pt_4->SetupAttachment(GetRootComponent());
	ImmutablePts.Add(Pt_4);

	Pt_5 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_5"));
	Pt_5->SetupAttachment(GetRootComponent());
	ImmutablePts.Add(Pt_5);

	Pt_6 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_6"));
	Pt_6->SetupAttachment(GetRootComponent());
	ImmutablePts.Add(Pt_6);

	Pt_7 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_7"));
	Pt_7->SetupAttachment(GetRootComponent());
	ImmutablePts.Add(Pt_7);

	Pt_8 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_8"));
	Pt_8->SetupAttachment(GetRootComponent());
	ImmutablePts.Add(Pt_8);

	Pt_9 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_9"));
	Pt_9->SetupAttachment(GetRootComponent());
	ImmutablePts.Add(Pt_9);

	Pt_10 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_10"));
	Pt_10->SetupAttachment(GetRootComponent());
	ImmutablePts.Add(Pt_10);
}

void APointCollection::BeginPlay()
{
	Super::BeginPlay();	
}

TArray<USceneComponent*> APointCollection::GetGroundPoints(const FVector& GroundLocation, int32 NumPoints, float YawOverride) const
{
	checkf(ImmutablePts.Num() >= NumPoints, TEXT("Attempted to access ImmutablePts out of bounds"));

	TArray<USceneComponent*> ArrayCopy;
	ArrayCopy.Reserve(NumPoints);

	for (USceneComponent* Pt : ImmutablePts)
	{
		if (ArrayCopy.Num() >= NumPoints) return ArrayCopy;

		if (Pt != Pt_0)
		{
			// 포인트 회전
			FVector ToPoint = Pt->GetComponentLocation() - Pt_0->GetComponentLocation();
			ToPoint = ToPoint.RotateAngleAxis(YawOverride, FVector::UpVector);
			Pt->SetWorldLocation(Pt_0->GetComponentLocation() + ToPoint);
		}

		// 충돌 체크로 바닥에 닿는 위치 확인
		const FVector RaisedLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, Pt->GetComponentLocation().Z + 500.f);
		const FVector LoweredLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, Pt->GetComponentLocation().Z - 500.f);

		FHitResult HitResult;
		TArray<AActor*> IgnoreActors;
		UAuraAbilitySystemLibrary::GetLivePlayersWithRadius(this, IgnoreActors, TArray<AActor*>(), 1500.f, GetActorLocation());

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActors(IgnoreActors);
		GetWorld()->LineTraceSingleByProfile(HitResult, RaisedLocation, LoweredLocation, FName(TEXT("BlockAll")), QueryParams);

		const FVector AdjustedLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, HitResult.ImpactPoint.Z);
		Pt->SetWorldLocation(AdjustedLocation);
		Pt->SetWorldRotation(UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal));

		ArrayCopy.Add(Pt);
	}

	return ArrayCopy;
}
