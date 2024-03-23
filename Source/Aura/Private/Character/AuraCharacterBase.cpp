// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/AuraCharacterBase.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// �浹üũ�� �޽����� �ٷ�
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), TEXT("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraCharacterBase::Die()
{
	// ĳ���Ͱ� �׾��� �� ���������� ������ ��

	// ���⸦ ����߸��°� ���ø����̼ǵǹǷ� ��Ƽĳ��Ʈ���� ���ص���
	if (Weapon)
	{
		Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	}
	
	MulticastHandleDeath();
}

FVector AAuraCharacterBase::GetCombatSocketLocation() const
{
	check(Weapon);
	
	return Weapon->GetSocketLocation(WeaponTipSocketName);
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation() const
{
	return HitReactMontage;
}

void AAuraCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(GetAbilitySystemComponent());
	check(GameplayEffectClass);

	// �����÷��� ����Ʈ�� Ȱ���� ��Ʈ����Ʈ ��Ʈ �ʱ�ȭ
	// �ʱ�ȭ�� ���������� �ϸ�ȴ�. ��Ʈ����Ʈ ��Ʈ�� �ڵ������� ���ø����̼� �ȴ�.
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	// ����Ʈ�� ����Ų ��ü ���
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void AAuraCharacterBase::AddCharacterAbilities()
{
	UAuraAbilitySystemComponent* AuraAbilitySystemComponent = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);

	// �����Ƽ�� �ο��� ������ �ؾ��Ѵ�. Ŭ����� ���ø����̼ǵ� ������ ���
	if (!HasAuthority()) return;

	// �����Ƽ �ý��� ������Ʈ�� �����Ƽ �ο�
	AuraAbilitySystemComponent->AddCharacterAbilities(StartupAbilites);
}

void AAuraCharacterBase::Dissolve()
{
	// �޽ÿ� ���⿡ ������� ���� ������ ��Ƽ���� ȿ���� �ο�
	if (GetMesh() && IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMatInst);

		StartDissolveTimeline(DynamicMatInst);
	}

	if (Weapon && IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, DynamicMatInst);

		StartWeaponDissolveTimeline(DynamicMatInst);
	}
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation()
{
	// ĳ���Ͱ� �׾��� �� ������ Ŭ�� ��� ������ ��

	// ���⸦ ���׵��� �ٲ۴�.
	if (Weapon)
	{
		Weapon->SetSimulatePhysics(true);
		Weapon->SetEnableGravity(true);
		Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}

	// ��ü�� ���׵�
	if (GetMesh())
	{
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetEnableGravity(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Block);
	}

	// �浹ó�� ����
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// ��Ƽ����� �޽ÿ� ���Ⱑ ������� ȿ��
	Dissolve();
}
