// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/AuraSummonAbility.h"
#include "Kismet/KismetSystemLibrary.h"

UAuraSummonAbility::UAuraSummonAbility()
	: NumMinions(5)
	, MinSpawnDistance(150.f)
	, MaxSpawnDistance(400.f)
	, SpawnSpread(90.f)
	, bDrawDebug(false)
{
}

TArray<FVector> UAuraSummonAbility::GetSpawnLocations() const
{
	if (!GetAvatarActorFromActorInfo()) return TArray<FVector>();
	if (!GetWorld()) return TArray<FVector>();

	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();

	// 부채꼴을 소환할 유닛 개수로 나눈다.
	const float DeltaSpread = SpawnSpread / NumMinions;

	// 부채꼴의 왼쪽 방향 벡터
	const FVector RightOfSpread = Forward.RotateAngleAxis(SpawnSpread / 2.f, FVector::UpVector);
	if (bDrawDebug) UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + RightOfSpread * MaxSpawnDistance, 4.f, FLinearColor::Green, 3.f);

	// 부채꼴의 오른쪽 방향 벡터
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);
	if (bDrawDebug) UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + LeftOfSpread * MaxSpawnDistance, 4.f, FLinearColor::Gray, 3.f);

	TArray<FVector> SpawnLocations;
	SpawnLocations.SetNum(NumMinions);

	for (int32 i = 0; i < NumMinions; ++i)
	{
		// 부채꼴 사이
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		if (bDrawDebug) UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + Direction * MaxSpawnDistance, 4.f, FLinearColor::Green, 3.f);

		// min ~ max 사거리 중 하나를 고름
		FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);

		// 언덕과 같은 곳에서 문제가 없도록 라인트레이스를 통해 부딪히는 지형 좌표를 확인
		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, ChosenSpawnLocation + FVector(0.f, 0.f, 400.f), ChosenSpawnLocation - FVector(0.f, 0.f, 400.f), ECollisionChannel::ECC_Visibility);
		if (Hit.bBlockingHit)
		{
			ChosenSpawnLocation = Hit.ImpactPoint;
		}

		if (bDrawDebug) DrawDebugSphere(GetWorld(), ChosenSpawnLocation, 18.f, 12, FColor::Cyan, false, 3.f);

		SpawnLocations[i] = ChosenSpawnLocation;
	}

	return SpawnLocations;
}
