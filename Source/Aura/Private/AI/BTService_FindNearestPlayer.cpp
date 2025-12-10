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
		// 타겟으로 잡아야할 태그를 설정
		const FName TargetTag = OwningPawn->ActorHasTag(TEXT("Player")) ? TEXT("Enemy") : TEXT("Player");

		// 해당 태그를 가진 모든 액터를 가져온다.
		TArray<AActor*> ActorsWithTag;
		UGameplayStatics::GetAllActorsWithTag(OwningPawn, TargetTag, ActorsWithTag);

		// 가장 가까운 타겟을 찾음
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

		// 블랙보드에 값 세팅
		if (ClosetActor)
		{
			UBTFunctionLibrary::SetBlackboardValueAsObject(this, TargetToFollowSelector, ClosetActor);
			UBTFunctionLibrary::SetBlackboardValueAsFloat(this, DistanceToTargetSelector, ClosetDistance);
		}
	}
}
