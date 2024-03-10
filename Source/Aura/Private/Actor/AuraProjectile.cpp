// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/AuraProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Aura/Aura.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AAuraProjectile::AAuraProjectile()
	: LifeSpan(15.f)
	, bHit(false)
{
	PrimaryActorTick.bCanEverTick = false;
	// ������ ���������� �� �Ŷ� Ŭ����� ���ؼ� ���ø����̼�
	bReplicates = true;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();	

	SetLifeSpan(LifeSpan);

	if (Sphere)
	{
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
	}

	if (LoopingSound)
	{
		LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
	}
}

void AAuraProjectile::Destroyed()
{
	// Ŭ���̾�Ʈ���� ������ �߻�ü�� �����϶�� ���ø����̼� �ߴµ�, ����� ����Ʈ�� ������� ���� ���
	if (!bHit && !HasAuthority())
	{
		if (ImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
		}

		if (ImpactEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
		}

		if (LoopingSoundComponent)
		{
			LoopingSoundComponent->Stop();
		}
	}

	Super::Destroyed();
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	if (ImpactEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	}

	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
	}

	// �������� �����ϴµ� Ŭ��� ���ø����̼ǵǼ� ���ŵȴ�.
	if (HasAuthority())
	{
		// Ÿ���� �����Ƽ �ý��� ������Ʈ�� ���ؼ� ������ ����Ʈ�� ����
		if (UAbilitySystemComponent* TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}

		Destroy();
	}
	// �ٵ� Ŭ�󿡼��� ���ø����̼ǵǼ� ���ŵ� �� ���峪 ����Ʈ�� ������ϰ� �������� �����϶�� �ؼ� ���ŵ� �� �ִ�.
	else
	{
		bHit = true;
	}
}
