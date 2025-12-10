// Fill out your copyright notice in the Description page of Project Settings.

#include "Checkpoint/Checkpoint.h"
#include "Components/SphereComponent.h"
#include "Interaction/PlayerInterface.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"

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

	MoveToComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MoveToComponent"));
	MoveToComponent->SetupAttachment(GetRootComponent());

	// 하이라이트될 때 색깔 설정
	if (CheckpointMesh)
	{
		CheckpointMesh->SetCustomDepthStencilValue(CustomDepthStencilOverride);
		CheckpointMesh->MarkRenderStateDirty();
	}
}

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	if (bBindOverlapCallback && Sphere)
	{
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
	}
}

void ACheckpoint::LoadActor_Implementation()
{
	// 저장되었다면 이펙트를 킴
	if (bReached)
	{
		HandleGlowEffects();
	}
}

void ACheckpoint::HighlightActor_Implementation()
{
	if (!CheckpointMesh) return;
	if (bReached) return;

	CheckpointMesh->SetRenderCustomDepth(true);
}

void ACheckpoint::UnHighlightActor_Implementation()
{
	if (!CheckpointMesh) return;

	CheckpointMesh->SetRenderCustomDepth(false);
}

void ACheckpoint::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	if (!MoveToComponent) return;

	OutDestination = MoveToComponent->GetComponentLocation();
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

void ACheckpoint::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->Implements<UPlayerInterface>())
	{
		bReached = true;

		// 현재 게임 상태를 저장
		if (AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			const UWorld* World = GetWorld();
			FString MapName = World->GetMapName();
			MapName.RemoveFromStart(World->StreamingLevelsPrefix);

			AuraGameModeBase->SaveWorldState(GetWorld(), MapName);
		}

		// 최신 게임 진행상황 저장
		IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);

		// 메쉬의 머티리얼이 빛나도록 변경
		HandleGlowEffects();
	}
}
