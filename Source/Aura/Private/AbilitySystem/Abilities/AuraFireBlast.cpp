// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/AuraFireBlast.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraFireBall.h"

FString UAuraFireBlast::GetDescription(int32 Level) const
{
	// ȭ�� ���� ��ų�� ���� ����

	// ������ ���� ����Ǵ� ������(�ҼӼ��� ����)
	const int32 ScaledDamage = static_cast<int32>(Damage.GetValueAtLevel(static_cast<float>(Level)));
	// ���� �ڽ�Ʈ
	const float ManaCost = FMath::Abs(GetManaCost(static_cast<float>(Level)));
	// ��Ÿ��
	const float Cooldown = GetCooldown(static_cast<float>(Level));

	return FString::Printf(TEXT(
		// ��ų��
		"<Title>FIRE BLAST</>\n\n"
		// ��ų �ɷ�ġ
		"<Small>Level: </><Level>%d</>\n"
		"<Small>ManaCost: </><Manacost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		// ����
		"<Default>Launches %d</>"
		"<Default>fire balls in all directions, each coming back and</>"
		"<Default>exploding upon return, causing </>"
		// ������
		"<Damage>%d</><Default> radial fire damage with"
		" a chance to burn</>\n\n"),
		Level, ManaCost, Cooldown, NumFireBolls, ScaledDamage);
}

FString UAuraFireBlast::GetNextLevelDescription(int32 Level) const
{
	const int32 ScaledDamage = static_cast<int32>(Damage.GetValueAtLevel(static_cast<float>(Level)));
	const float ManaCost = FMath::Abs(GetManaCost(static_cast<float>(Level)));
	const float Cooldown = GetCooldown(static_cast<float>(Level));

	return FString::Printf(TEXT(
		// ��ų��
		"<Title>NEXT LEVEL: </>\n\n"
		// ��ų �ɷ�ġ
		"<Small>Level: </><Level>%d</>\n"
		"<Small>ManaCost: </><Manacost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		// ����
		"<Default>Launches %d</>"
		"<Default>fire balls in all directions, each coming back and</>"
		"<Default>exploding upon return, causing </>"
		// ������
		"<Damage>%d</><Default> radial fire damage with"
		" a chance to burn</>\n\n"),
		Level, ManaCost, Cooldown, NumFireBolls, ScaledDamage);
}

TArray<AAuraFireBall*> UAuraFireBlast::SpawnFireBalls()
{
	TArray<AAuraFireBall*> FireBalls;

	if (!GetWorld()) return FireBalls;
	if (!CurrentActorInfo) return FireBalls;
	if (!GetAvatarActorFromActorInfo()) return FireBalls;

	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();

	// 360�� ���������� ���� ���͵��� ����
	const FVector Foward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::EvenlySpreadRotators(Foward, FVector::UpVector, 360.f, NumFireBolls);

	// ���⸶�� ȭ������ ����
	for (const FRotator& Rotator : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(Location);
		SpawnTransform.SetRotation(Rotator.Quaternion());

		if (AAuraFireBall* FireBall = GetWorld()->SpawnActorDeferred<AAuraFireBall>(FireBallClass, SpawnTransform, GetOwningActorFromActorInfo(), CurrentActorInfo->PlayerController->GetPawn(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn))
		{
			FireBall->SetOwner(GetAvatarActorFromActorInfo());
			FireBall->DamageEffectParams = MakeDamageEffectParamsFromClassDefault();
			FireBall->SetReturnToActor(GetAvatarActorFromActorInfo());

			FDamageEffectParams ExplosionDamageEffectParams = MakeDamageEffectParamsFromClassDefault();

			FireBalls.Add(FireBall);
			FireBall->FinishSpawning(SpawnTransform);
		}
	}
	return FireBalls;
}
