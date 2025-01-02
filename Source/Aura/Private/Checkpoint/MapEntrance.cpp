// Fill out your copyright notice in the Description page of Project Settings.

#include "Checkpoint/MapEntrance.h"
#include "Interaction/PlayerInterface.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"

void AMapEntrance::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->Implements<UPlayerInterface>())
	{
		bReached = true;

		// 현재 게임 상태를 저장
		if (AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			AuraGameModeBase->SaveWorldState(GetWorld());
		}

		// 최신 게임 진행상황 저장
		IPlayerInterface::Execute_SaveProgress(OtherActor, DestinationPlayerStartTag);

		// 메쉬의 머티리얼이 빛나도록 변경
		HandleGlowEffects();
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
