// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;
class UBehaviorTree;
class AAuraAIController;

/**
 * 몬스터 캐릭터 클래스
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()
	
private:
	AAuraEnemy();

private:
	virtual void BeginPlay() final;
	virtual void PossessedBy(AController* NewController) final;

	// AAuraCharacterBase에서 상속
	virtual void InitAbilityActorInfo() final;
	virtual void InitializeDefaultAttributes() const final;

	// ICombatInterface에서 상속
	virtual void Die() final;
	virtual int32 GetPlayerLevel_Implementation() const final;

	// IEnemyInterface에서 상속
	virtual void HighlightActor() final;
	virtual void UnHighlightActor() final;
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) final;
	virtual AActor* GetCombatTarget_Implementation() const final;

private:
	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

public:
	// 오버레이 위젯 컨트롤러에서 가져온 체력 변경될 때 적용할 델리게이트
	// 위젯 컨트롤러에서 구현하는 것 뿐 아니라 위젯 컨트롤러는 단순한 UObject이므로 이렇게 액터에서도 바로 UI로 연결할 수 있다.
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

protected:
	// 체력바 UI
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	// 레벨
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;

	// 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float BaseWalkSpeed = 250.f;

	// 생존 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifeSpan = 5.f;

	// 피격 모션 중인지?
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting = false;

	// 공격 타겟들
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TObjectPtr<AActor> CombatTarget;

private:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	// 참조 변수
	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController;
};
