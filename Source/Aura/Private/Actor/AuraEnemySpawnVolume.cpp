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

	// ���� ���ؼ��� ����
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
	// �÷��̾ ���� �� ������ �ٽ� ��ȯ���� �ʵ��� �ε����ڸ��� �ٷ� ������
	if (bReached)
	{
		Destroy();
	}
}

void AAuraEnemySpawnVolume::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	// �÷��̾�Ը� ����
	if (!OtherActor->Implements<UPlayerInterface>()) return;

	bReached = true;

	// ��� ����Ʈ���� ���� ��ȯ�ϰ�
	for (const AAuraEnemySpawnPoint* Point : SpawnPoints)
	{
		if (IsValid(Point))
		{
			Point->SpawnEnemy();
		}
	}

	// ��ȯ�� ������ �浹ó�� ��ȿ
	if (Box)
	{
		Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
