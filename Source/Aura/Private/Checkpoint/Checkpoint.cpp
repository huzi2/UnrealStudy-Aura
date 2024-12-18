// Fill out your copyright notice in the Description page of Project Settings.

#include "Checkpoint/Checkpoint.h"
#include "Components/SphereComponent.h"

ACheckpoint::ACheckpoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CheckpointMesh"));
	CheckpointMesh->SetupAttachment(GetRootComponent());
	CheckpointMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CheckpointMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(CheckpointMesh);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	if (Sphere)
	{
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
	}
}

void ACheckpoint::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 플레이어와 충돌하면 메쉬의 머티리얼 변경
	if (OtherActor && OtherActor->ActorHasTag(TEXT("Player")))
	{
		HandleGlowEffects();
	}
}

void ACheckpoint::HandleGlowEffects()
{
	if (!Sphere) return;
	if (!CheckpointMesh) return;

	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(CheckpointMesh->GetMaterial(0), this);
	CheckpointMesh->SetMaterial(0, DynamicMaterialInstance);

	CheckpointReached(DynamicMaterialInstance);
}
