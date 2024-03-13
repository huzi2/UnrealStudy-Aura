// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()
	
private:
	AAuraEnemy();

private:
	virtual void BeginPlay() final;

	virtual void InitAbilityActorInfo() final;
	virtual void InitializeDefaultAttributes() const final;

	virtual void Die() final;
	virtual int32 GetPlayerLevel() const final;

	virtual void HighlightActor() final;
	virtual void UnHighlightActor() final;

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	ECharacterClass CharacterClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifeSpan;
};
