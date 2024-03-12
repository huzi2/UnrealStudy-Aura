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
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface) return;

	const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	Rotation.Pitch = 0.f;

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
			// 발사체의 이펙트 스펙 핸들에 적용
			Projectile->DamageEffectSpecHandle = SourceAbilitySystemComponent->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), SourceAbilitySystemComponent->MakeEffectContext());

			// 발사체의 메타 어트리뷰트 Damage에 데미지값 적용
			const UAuraGameplayTags GameplayTags = UAuraGameplayTags::Get();
			// 어빌리티 레벨에 따라 커브테이블에 적용된 데미지를 얻어온다.
			const float ScaledDamage = Damage.GetValueAtLevel(static_cast<float>(GetAbilityLevel()));
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(Projectile->DamageEffectSpecHandle, GameplayTags.Damage, ScaledDamage);
		}
	}
	
	// 발사체 이제 생성
	Projectile->FinishSpawning(SpawnTransform);
}
