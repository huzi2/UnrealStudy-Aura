// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "Interaction/HighlightInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;
class UBehaviorTree;
class AAuraAIController;

/**
 * ���� ĳ���� Ŭ����
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface, public IHighlightInterface
{
	GENERATED_BODY()
	
private:
	AAuraEnemy();

private:
	virtual void BeginPlay() final;
	virtual void PossessedBy(AController* NewController) final;

	// AAuraCharacterBase���� ���
	virtual void InitAbilityActorInfo() final;
	virtual void InitializeDefaultAttributes() const final;
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount) final;

	// ICombatInterface���� ���
	virtual void Die(const FVector& DeathImpulse) final;
	virtual int32 GetPlayerLevel_Implementation() const final;

	// IEnemyInterface���� ���
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) final;
	virtual AActor* GetCombatTarget_Implementation() const final;

	// IHighlightInterface���� ���
	virtual void HighlightActor_Implementation() final;
	virtual void UnHighlightActor_Implementation() final;
	virtual void SetMoveToLocation_Implementation(FVector& OutDestination) final;

public:
	FORCEINLINE void SetLevel(int32 InLevel) { Level = InLevel; }

protected:
	// ������ ���
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnLoot();

private:
	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

public:
	// �������� ���� ��Ʈ�ѷ����� ������ ü�� ����� �� ������ ��������Ʈ
	// ���� ��Ʈ�ѷ����� �����ϴ� �� �� �ƴ϶� ���� ��Ʈ�ѷ��� �ܼ��� UObject�̹Ƿ� �̷��� ���Ϳ����� �ٷ� UI�� ������ �� �ִ�.
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

protected:
	// ü�¹� UI
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	// ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;

	// ���� �ð�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifeSpan = 5.f;

	// �ǰ� ��� ������?
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting = false;

	// ���� Ÿ�ٵ�
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TObjectPtr<AActor> CombatTarget;

private:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	// ���� ����
	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController;
};
