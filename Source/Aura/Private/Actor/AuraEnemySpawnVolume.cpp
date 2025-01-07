// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/AuraEnemySpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Interaction/PlayerInterface.h"
#include "Actor/AuraEnemySpawnPoint.h"

AAuraEnemySpawnVolume::AAuraEnemySpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	SetRootComponent(Box);

	// 폰에 대해서만 반응
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionObjectType(ECC_WorldStatic);
	Box->SetCollisionResponseToAllChannels(ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AAuraEnemySpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	if (Box)
	{
		Box->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBoxOverlap);
	}
}

void AAuraEnemySpawnVolume::LoadActor_Implementation()
{
	// 플레이어가 닿은 적 있으면 다시 소환하지 않도록 로드하자마자 바로 삭제함
	if (bReached)
	{
		Destroy();
	}
}

void AAuraEnemySpawnVolume::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	// 플레이어에게만 반응
	if (!OtherActor->Implements<UPlayerInterface>()) return;

	bReached = true;

	// 모든 포인트에서 적을 소환하고
	for (const AAuraEnemySpawnPoint* Point : SpawnPoints)
	{
		if (IsValid(Point))
		{
			Point->SpawnEnemy();
		}
	}

	// 소환이 끝나면 충돌처리 무효
	if (Box)
	{
		Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
