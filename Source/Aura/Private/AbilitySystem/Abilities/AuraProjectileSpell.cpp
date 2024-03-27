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

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	if (!GetAvatarActorFromActorInfo()) return;

	// 발사체 생성은 서버에서만 진행
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;
	if (!GetWorld()) return;

	// 액터 관련값 떄문에 어빌리티가 액터 클래스에 종속되지 않기 위해서 인터페이스를 사용한다.
	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo());
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();

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

			// 발사체의 메타 어트리뷰트 Damage에 데미지값 적용
			const UAuraGameplayTags GameplayTags = UAuraGameplayTags::Get();

			// 데미지 타입별로 적용
			for (const auto& Pair : DamageTypes)
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
