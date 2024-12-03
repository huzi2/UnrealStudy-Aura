// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LoadScreenHUD.generated.h"

class ULoadScreenWidget;
class UMVVM_LoadScreen;

/**
 * �ε� �޴����� ����� HUD Ŭ����
 */
UCLASS()
class AURA_API ALoadScreenHUD : public AHUD
{
	GENERATED_BODY()
	
private:
	virtual void BeginPlay() final;

protected:
	// �ε� �޴� UI ��ü
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ULoadScreenWidget> LoadScreenWidget;

	// �ε� �޴� ��� ��ü
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMVVM_LoadScreen> LoadScreenViewModel;

private:
	// �ε� �޴� UI Ŭ����
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> LoadScreenWidgetClass;

	// �ε� �޴� ��� Ŭ����
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadScreen> LoadScreenViewModelClass;
};
