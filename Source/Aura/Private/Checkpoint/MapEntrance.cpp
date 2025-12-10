// Fill out your copyright notice in the Description page of Project Settings.

#include "Checkpoint/MapEntrance.h"
#include "Interaction/PlayerInterface.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"

AMapEntrance::AMapEntrance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (Sphere)
	{
		Sphere->SetupAttachment(MoveToComponent);
	}
}

void AMapEntrance::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->Implements<UPlayerInterface>())
	{
		bReached = true;

		// 현재 게임 상태를 저장
		if (AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			AuraGameModeBase->SaveWorldState(GetWorld(), DestinationMap.ToSoftObjectPath().GetAssetName());
		}

		// 최신 게임 진행상황 저장
		IPlayerInterface::Execute_SaveProgress(OtherActor, DestinationPlayerStartTag);

		// 해당 맵으로 이동
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, DestinationMap);
	}
}

void AMapEntrance::LoadActor_Implementation()
{
	// 로드해도 아무것도 안함
}

void AMapEntrance::HighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(true);
}
