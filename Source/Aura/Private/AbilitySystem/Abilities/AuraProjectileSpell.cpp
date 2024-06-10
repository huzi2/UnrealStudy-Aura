// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

FString UAuraProjectileSpell::GetDescription(int32 Level) const
{
	// 파이어볼 스킬에 대한 설명

	// 레벨에 따라 변경되는 데미지(불속성만 얻어옴)
	const int32 Damage = static_cast<int32>(DamageTypes[UAuraGameplayTags::Get().Damage_Fire].GetValueAtLevel(Level));

	// 1레벨은 하나만 발사
	if (Level == 1)
	{
		return FString::Printf(TEXT("<Title>FIRE BOLT</>\n\n<Default>Launched a bolt of fire, exploding on impact and dealing: </><Damage>%d</><Default> fire damage with a chance to burn</>\n\n<Small>Level: </><Level>%d</>"), Damage, Level);
	}
	// 그 후에는 레벨에 따라 발사체가 증가
	else
	{
		return FString::Printf(TEXT("<Title>FIRE BOLT</>\n\n<Default>Launched %d bolts of fire, exploding on impact and dealing: </><Damage>%d</><Default> fire damage with a chance to burn</>\n\n<Small>Level: </><Level>%d</>"), FMath::Min(Level, NumProjectiles), Damage, Level);
	}
}

FString UAuraProjectileSpell::GetNextLevelDescription(int32 Level) const
{
	const int32 Damage = static_cast<int32>(DamageTypes[UAuraGameplayTags::Get().Damage_Fire].GetValueAtLevel(Level));
	return FString::Printf(TEXT("<Title>NEXT LEVEL: </>\n\n<Default>Launched %d bolts of fire, exploding on impact and dealing: </><Damage>%d</><Default> fire damage with a chance to burn</>\n\n<Small>Level: </><Level>%d</>"), FMath::Min(Level, NumProjectiles), Damage, Level);
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride)
{
	if (!GetAvatarActorFromActorInfo()) return;

	// 발사체 생성은 서버에서만 진행
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;
	if (!GetWorld()) return;

	// 액터 관련값 떄문에 어빌리티가 액터 클래스에 종속되지 않기 위해서 인터페이스를 사용한다.
	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();

	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());

	// 게임플레이 이펙트를 적용시켜야 하므로 SpawnActorDeferred()로 생성을 미룬다.
	AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform, GetOwningActorFromActorInfo(), Cast<APawn>(GetOwningActorFromActorInfo()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!Projectile) return;

	// 액터의 어빌리티 시스템 컴포넌트를 사용해서 어빌리티에 적용할 게임플레이 이펙트 스펙 핸들을 생성
	if (const UAbilitySystemComponent* SourceAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo()))
	{
		if (DamageEffectClass)
		{
			// 이펙트의 정보를 채우기 위해 게임플레이 컨텍스트 핸들을 생성
			FGameplayEffectContextHandle EffectContextHandle = SourceAbilitySystemComponent->MakeEffectContext();
			// 아래 목록은 게임플레이 컨텍스트 핸들로 가능한 것들을 모아놓은 것. 이 게임에서는 의미는 없다.
			// 이펙트를 발생시킨 어빌리티 등록
			EffectContextHandle.SetAbility(this);
			// 이펙트를 발생시킨 주체(발사체) 등록
			EffectContextHandle.AddSourceObject(Projectile);
			// 이펙트와 관련된 액터들 정보에 발사체 등록
			TArray<TWeakObjectPtr<AActor>> Actors;
			Actors.Add(Projectile);
			EffectContextHandle.AddActors(Actors);
			// 충돌처리 관련 결과가 있을 때 충돌처리 결과를 전달(현재는 비어있음)
			FHitResult HitResult;
			HitResult.Location = ProjectileTargetLocation;
			EffectContextHandle.AddHitResult(HitResult);

			// 발사체의 이펙트 스펙 핸들에 적용
			Projectile->DamageEffectSpecHandle = SourceAbilitySystemComponent->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

			// 데미지 타입별로 적용
			for (const TTuple<FGameplayTag, FScalableFloat>& Pair : DamageTypes)
			{
				// 어빌리티 레벨에 따라 커브테이블에 적용된 데미지를 얻어온다.
				const float ScaledDamage = Pair.Value.GetValueAtLevel(static_cast<float>(GetAbilityLevel()));

				UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(Projectile->DamageEffectSpecHandle, Pair.Key, ScaledDamage);
			}
		}
	}
	
	// 발사체 이제 생성
	Projectile->FinishSpawning(SpawnTransform);
}
