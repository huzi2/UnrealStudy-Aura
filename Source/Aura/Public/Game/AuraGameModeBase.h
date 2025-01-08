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
 * 게임모드 클래스
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

	// 뷰모델을 통한 게임 데이터 저장
	void SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex);

	// 해당 슬롯에 저장된 세이브 게임오브젝트 확인
	ULoadScreenSaveGame* GetSaveLoadData(const FString& SlotName, int32 SlotIndex) const;

	// 슬롯 삭제
	static void DeleteSlot(const FString& SlotName, int32 SlotIndex);

	// 슬롯의 맵으로 이동
	void TravelToMap(UMVVM_LoadSlot* Slot);

	// 게임 인스턴스에 저장된 세이브 데이터 확인
	ULoadScreenSaveGame* RetrieveInGameSaveData() const;
	// 확인한 세이브 데이터에 현재 플레이어 게임 상황을 저장
	void SaveInGameProgressData(ULoadScreenSaveGame* SaveObject);
	// 전체 게임 상황을 저장
	void SaveWorldState(UWorld* World, const FString& DestinationMapAssetName = FString("")) const;
	// 게임 상황을 로드
	void LoadWorldState(UWorld* World) const;

	// 플레이어가 죽었을 때 처리
	void PlayerDied(ACharacter* DeadCharacter);

private:
	virtual void BeginPlay() override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

private:
	// 맵 에셋 이름으로 맵 이름 찾기
	FString GetMapNameFromMapAssetName(const FString& MapAssetName) const;

private:
	// 모든 직업 정보를 가진 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	// 플레이어의 모든 스킬 정보를 가진 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	// 세이브 게임 오브젝트 클래스
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ULoadScreenSaveGame> LoadScreenSaveGameClass;

	// 게임 시작할 때 사용할 기본 맵 이름
	UPROPERTY(EditDefaultsOnly)
	FString DefaultMapName;
	// 게임 시작할 때 사용할 기본 맵
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> DefaultMap;
	// 게임 시작할 때 위치로 잡을 기본 플레이어 스타트의 태그
	UPROPERTY(EditDefaultsOnly)
	FName DefaultPlayerStartTag;
	// 게임에 사용할 모든 맵들
	UPROPERTY(EditDefaultsOnly)
	TMap<FString, TSoftObjectPtr<UWorld>> Maps;
};
