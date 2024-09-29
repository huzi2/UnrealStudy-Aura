// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/AuraBeamSpell.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

void UAuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.ImpactPoint;
		MouseHitActor = HitResult.GetActor();
	}
	else
	{
		// Ÿ���� ������ �����Ƽ ���
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

			// ���� ���Ͽ��� Ÿ�� ��ġ���� Ʈ���̽�
			FHitResult HitResult;
			UKismetSystemLibrary::SphereTraceSingle(OwnerCharcter, SocketLocation, BeamTargetLocation, 10.f, TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);

			// �浹�� Ÿ���� ������� ����(���콺 ��ġ ��ó�� ���� �ڵ� ������)
			if (HitResult.bBlockingHit)
			{
				MouseHitLocation = HitResult.ImpactPoint;
				MouseHitActor = HitResult.GetActor();
			}
		}
	}
}

void UAuraBeamSpell::StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets)
{
	if (MouseHitActor == nullptr) return;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Reserve(2);
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	ActorsToIgnore.Add(MouseHitActor);

	TArray<AActor*> OverlappingActors;
	UAuraAbilitySystemLibrary::GetLivePlayersWithRadius(GetAvatarActorFromActorInfo(), OverlappingActors, ActorsToIgnore, 850.f, MouseHitActor->GetActorLocation());

	//int32 NumAdditionalTargets = FMath::Min(GetAbilityLevel() - 1, MaxNumShockTargets);
	int32 NumAdditionalTargets = 5;

	// ���� ����� ������ Ÿ�ٵ��� ����
	UAuraAbilitySystemLibrary::GetClosestTargets(NumAdditionalTargets, OverlappingActors, OutAdditionalTargets, MouseHitActor->GetActorLocation());
}
