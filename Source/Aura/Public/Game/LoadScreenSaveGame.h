// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

UENUM(BlueprintType)
enum ESaveSlotStatus
{
	Vacant,
	EnterName,
	Taken
};

/**
 * ������ ���� ������
 */
UCLASS()
class AURA_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	// ���̺� ���� ���� ���� ����
	UPROPERTY()
	FString SlotName = FString();
	UPROPERTY()
	FString PlayerName = FString(TEXT("Default Nmae"));
	UPROPERTY()
	FString MapName = FString(TEXT("Default Map Nmae"));
	UPROPERTY()
	int32 SlotIndex = 0;
	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = ESaveSlotStatus::Vacant;
	UPROPERTY()
	FName PlayerStartTag;
	UPROPERTY()
	bool bFirstTimeLoadIn = true;

	// �÷��̾� ���� ���� ����
	UPROPERTY()
	int32 PlayerLevel = 1;
	UPROPERTY()
	int32 XP = 0;
	UPROPERTY()
	int32 AttributePoints = 0;
	UPROPERTY()
	int32 SpellPoints = 0;

	// �÷��̾� �⺻ �ɷ�ġ ���� ����
	UPROPERTY()
	float Strength = 0.f;
	UPROPERTY()
	float Intelligence = 0.f;
	UPROPERTY()
	float Resilience = 0.f;
	UPROPERTY()
	float Vigor = 0.f;
};
