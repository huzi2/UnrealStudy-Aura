// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/AuraBeamSpell.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.ImpactPoint;
		MouseHitActor = HitResult.GetActor();
	}
	else
	{
		// 타겟이 없으면 어빌리티 취소
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UAuraBeamSpell::StoreOwnerVariables()
{
	if (CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharcter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}

void UAuraBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	check(OwnerCharcter);

	if (OwnerCharcter->Implements<UCombatInterface>())
	{
		if (USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeapon(OwnerCharcter))
		{
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(OwnerCharcter);

			const FVector SocketLocation = Weapon->GetSocketLocation(FName(TEXT("TipSocket")));

			// 무기 소켓에서 타겟 위치까지 트레이스
			FHitResult HitResult;
			UKismetSystemLibrary::SphereTraceSingle(OwnerCharcter, SocketLocation, BeamTargetLocation, 10.f, TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);

			// 충돌한 타겟을 대상으로 삼음(마우스 위치 근처의 적을 자동 설정함)
			if (HitResult.bBlockingHit)
			{
				MouseHitLocation = HitResult.ImpactPoint;
				MouseHitActor = HitResult.GetActor();
			}
		}
	}
}
