// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AuraInputConfig.generated.h"

class UInputAction;

/**
 * ���� �Է°� �±׸� �����ϱ� ���� ����ü
 */
USTRUCT(BlueprintType)
struct FAuraInputAction
{
	GENERATED_BODY()

public:
	// ��ǲ �׼�
	UPROPERTY(EditDefaultsOnly)
	const UInputAction* InputAction = nullptr;
	// ��ǲ �±�
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag = FGameplayTag();
};

/**
 * ��ǲ �±׿� �׼� ���� Ŭ����
 */
UCLASS()
class AURA_API UAuraInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// ��ǲ �±׷� ��ǲ �׼� ã��
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = false) const;

public:
	// ��� ��ǲ �±�-�׼� ���� �����̳�
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAuraInputAction> AbilityInputActions;
};
