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

	// 게임 태그 관리하는 싱글톤 클래스의 초기화(필요한 태그를 태그매니저에 기입)
	UAuraGameplayTags::InitializeNativeGameplayTags();

	// 타겟 데이터와 같은 데이터 초기화. 타겟 데이터 쓰려면 반드시 해야함
	UAbilitySystemGlobals::Get().InitGlobalData();
}
