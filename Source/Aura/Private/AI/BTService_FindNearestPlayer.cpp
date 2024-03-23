// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BTService_FindNearestPlayer.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BTFunctionLibrary.h"

void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (!AIOwner) return;
	
	if (APawn* OwningPawn = AIOwner->GetPawn())
	{
		// Ÿ������ ��ƾ��� �±׸� ����
		const FName TargetTag = OwningPawn->ActorHasTag(TEXT("Player")) ? TEXT("Enemy") : TEXT("Player");

		// �ش� �±׸� ���� ��� ���͸� �����´�.
		TArray<AActor*> ActorsWithTag;
		UGameplayStatics::GetAllActorsWithTag(OwningPawn, TargetTag, ActorsWithTag);

		// ���� ����� Ÿ���� ã��
		float ClosetDistance = TNumericLimits<float>::Max();
		AActor* ClosetActor = nullptr;
		for (AActor* Actor : ActorsWithTag)
		{
			if (!Actor) continue;

			const float Distance = OwningPawn->GetDistanceTo(Actor);
			if (Distance < ClosetDistance)
			{
				ClosetDistance = Distance;
				ClosetActor = Actor;
			}
		}

		// �����忡 �� ����
		if (ClosetActor)
		{
			UBTFunctionLibrary::SetBlackboardValueAsObject(this, TargetToFollowSelector, ClosetActor);
			UBTFunctionLibrary::SetBlackboardValueAsFloat(this, DistanceToTargetSelector, ClosetDistance);
		}
	}
}
