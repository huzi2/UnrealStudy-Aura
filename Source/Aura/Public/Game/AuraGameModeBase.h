// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class UCharacterClassInfo;
class UAbilityInfo;
class UMVVM_LoadSlot;
class ULoadScreenSaveGame;

/**
 * ���Ӹ�� Ŭ����
 */
UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	FORCEINLINE UCharacterClassInfo* GetCharacterClassInfo() const { return CharacterClassInfo; }
	FORCEINLINE UAbilityInfo* GetAbilityInfo() const { return AbilityInfo; }
	FORCEINLINE const FString& GetDefaultMapName() const { return DefaultMapName; }
	FORCEINLINE const FName& GetDefaultPlayerStartTag() const { return DefaultPlayerStartTag; }
	FORCEINLINE const TSoftObjectPtr<UWorld>& GetDefaultMap() const { return DefaultMap; }

	// ����� ���� ���� ������ ����
	void SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex);

	// �ش� ���Կ� ����� ���̺� ���ӿ�����Ʈ Ȯ��
	ULoadScreenSaveGame* GetSaveLoadData(const FString& SlotName, int32 SlotIndex) const;

	// ���� ����
	static void DeleteSlot(const FString& SlotName, int32 SlotIndex);

	// ������ ������ �̵�
	void TravelToMap(UMVVM_LoadSlot* Slot);

	// ���� �ν��Ͻ��� ����� ���̺� ������ Ȯ��
	ULoadScreenSaveGame* RetrieveInGameSaveData() const;
	// Ȯ���� ���̺� �����Ϳ� ���� �÷��̾� ���� ��Ȳ�� ����
	void SaveInGameProgressData(ULoadScreenSaveGame* SaveObject);
	// ��ü ���� ��Ȳ�� ����
	void SaveWorldState(UWorld* World, const FString& DestinationMapAssetName = FString("")) const;
	// ���� ��Ȳ�� �ε�
	void LoadWorldState(UWorld* World) const;

	// �÷��̾ �׾��� �� ó��
	void PlayerDied(ACharacter* DeadCharacter);

private:
	virtual void BeginPlay() override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

private:
	// �� ���� �̸����� �� �̸� ã��
	FString GetMapNameFromMapAssetName(const FString& MapAssetName) const;

private:
	// ��� ���� ������ ���� Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	// �÷��̾��� ��� ��ų ������ ���� Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	// ���̺� ���� ������Ʈ Ŭ����
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ULoadScreenSaveGame> LoadScreenSaveGameClass;

	// ���� ������ �� ����� �⺻ �� �̸�
	UPROPERTY(EditDefaultsOnly)
	FString DefaultMapName;
	// ���� ������ �� ����� �⺻ ��
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> DefaultMap;
	// ���� ������ �� ��ġ�� ���� �⺻ �÷��̾� ��ŸƮ�� �±�
	UPROPERTY(EditDefaultsOnly)
	FName DefaultPlayerStartTag;
	// ���ӿ� ����� ��� �ʵ�
	UPROPERTY(EditDefaultsOnly)
	TMap<FString, TSoftObjectPtr<UWorld>> Maps;
};
