// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

/**
 * 저장할 게임 데이터
 */
UCLASS()
class AURA_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	FString SlotName = FString();
	UPROPERTY()
	FString PlayerName = FString(TEXT("Default Nmae"));
	UPROPERTY()
	int32 SlotIndex = 0;
};
