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

		// ���� ���� ���¸� ����
		if (AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			AuraGameModeBase->SaveWorldState(GetWorld());
		}

		// �ֽ� ���� �����Ȳ ����
		IPlayerInterface::Execute_SaveProgress(OtherActor, DestinationPlayerStartTag);

		// �޽��� ��Ƽ������ �������� ����
		HandleGlowEffects();
	}
}

void AMapEntrance::LoadActor_Implementation()
{
	// �ε��ص� �ƹ��͵� ����
}

void AMapEntrance::HighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(true);
}
