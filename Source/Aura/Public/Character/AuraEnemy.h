// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
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

	virtual int32 GetPlayerLevel() const final;

	virtual void HighlightActor() final;
	virtual void UnHighlightActor() final;

public:
	// �������� ���� ��Ʈ�ѷ����� ������ ü�� ����� �� ������ ��������Ʈ
	// ���� ��Ʈ�ѷ����� �����ϴ� �� �� �ƴ϶� ���� ��Ʈ�ѷ��� �ܼ��� UObject�̹Ƿ� �̷��� ���Ϳ����� �ٷ� UI�� ������ �� �ִ�.
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;
};
