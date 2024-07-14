// Fill out your copyright notice in the Description page of Project Settings.

#include "AuraAssetManager.h"
#include "AuraGameplayTags.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

const UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine);

	UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);
	return *AuraAssetManager;
}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	// ���� �±� �����ϴ� �̱��� Ŭ������ �ʱ�ȭ(�ʿ��� �±׸� �±׸Ŵ����� ����)
	UAuraGameplayTags::InitializeNativeGameplayTags();

	// Ÿ�� �����Ϳ� ���� ������ �ʱ�ȭ. Ÿ�� ������ ������ �ݵ�� �ؾ���
	UAbilitySystemGlobals::Get().InitGlobalData();
}
