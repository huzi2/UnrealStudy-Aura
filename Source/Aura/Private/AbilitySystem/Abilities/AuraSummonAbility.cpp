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

	// ��ä���� ��ȯ�� ���� ������ ������.
	const float DeltaSpread = SpawnSpread / NumMinions;

	// ��ä���� ���� ���� ����
	const FVector RightOfSpread = Forward.RotateAngleAxis(SpawnSpread / 2.f, FVector::UpVector);
	if (bDrawDebug) UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + RightOfSpread * MaxSpawnDistance, 4.f, FLinearColor::Green, 3.f);

	// ��ä���� ������ ���� ����
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);
	if (bDrawDebug) UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + LeftOfSpread * MaxSpawnDistance, 4.f, FLinearColor::Gray, 3.f);

	TArray<FVector> SpawnLocations;
	SpawnLocations.SetNum(NumMinions);

	for (int32 i = 0; i < NumMinions; ++i)
	{
		// ��ä�� ����
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		if (bDrawDebug) UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + Direction * MaxSpawnDistance, 4.f, FLinearColor::Green, 3.f);

		// min ~ max ��Ÿ� �� �ϳ��� ��
		FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);

		// ����� ���� ������ ������ ������ ����Ʈ���̽��� ���� �ε����� ���� ��ǥ�� Ȯ��
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
