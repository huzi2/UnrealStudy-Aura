// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "Game/LoadScreenSaveGame.h"
#include "Game/AuraGameInstance.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "Interaction/SaveInterface.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Aura/AuraLogChannels.h"

void AAuraGameModeBase::SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex)
{
	if (!LoadSlot) return;
	if (!LoadScreenSaveGameClass) return;
	
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);
	if (!LoadScreenSaveGame) return;

	// 세이브 슬롯이 중복될 경우 기존 걸 삭제
	DeleteSlot(LoadSlot->GetSlotName(), SlotIndex);

	// 데이터 저장
	LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
	LoadScreenSaveGame->MapName = LoadSlot->GetMapName();
	LoadScreenSaveGame->SaveSlotStatus = ESaveSlotStatus::Taken;
	LoadScreenSaveGame->PlayerStartTag = LoadSlot->GetPlayerStartTag();

	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->GetSlotName(), SlotIndex);
}

ULoadScreenSaveGame* AAuraGameModeBase::GetSaveLoadData(const FString& SlotName, int32 SlotIndex) const
{
	USaveGame* SaveGameObject = nullptr;

	// 슬롯에 저장되어있으면 그걸 리턴, 아니며 새로 만든다.
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		SaveGameObject = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
	}
	else
	{
		SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	}
	return Cast<ULoadScreenSaveGame>(SaveGameObject);
}

void AAuraGameModeBase::DeleteSlot(const FString& SlotName, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
	}
}

void AAuraGameModeBase::TravelToMap(UMVVM_LoadSlot* Slot)
{
	if (!Slot) return;

	const FString& SlotName = Slot->GetSlotName();
	const int32 SlotIndex = Slot->GetSlotIndex();

	UGameplayStatics::OpenLevelBySoftObjectPtr(Slot, Maps.FindChecked(Slot->GetMapName()));
}

ULoadScreenSaveGame* AAuraGameModeBase::RetrieveInGameSaveData() const
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	if (!AuraGameInstance) return nullptr;

	const FString& InGameLoadSlotName = AuraGameInstance->GetLoadSlotName();
	const int32 InGameLoadSlotIndex = AuraGameInstance->GetLoadSlotIndex();
	return GetSaveLoadData(InGameLoadSlotName, InGameLoadSlotIndex);
}

void AAuraGameModeBase::SaveInGameProgressData(ULoadScreenSaveGame* SaveObject)
{
	if (!SaveObject) return;

	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	if (!AuraGameInstance) return;

	const FString& InGameLoadSlotName = AuraGameInstance->GetLoadSlotName();
	const int32 InGameLoadSlotIndex = AuraGameInstance->GetLoadSlotIndex();

	// 게임 인스턴스에도 최신 내용 적용
	AuraGameInstance->SetPlayerStartTag(SaveObject->PlayerStartTag);

	UGameplayStatics::SaveGameToSlot(SaveObject, InGameLoadSlotName, InGameLoadSlotIndex);
}

void AAuraGameModeBase::SaveWorldState(UWorld* World, const FString& DestinationMapAssetName) const
{
	if (!World) return;

	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	if (!AuraGameInstance) return;

	ULoadScreenSaveGame* SaveObject = GetSaveLoadData(AuraGameInstance->GetLoadSlotName(), AuraGameInstance->GetLoadSlotIndex());
	if (!SaveObject) return;

	// 맵 저장
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	// 따로 설정한 맵 에셋이 있다면 저장
	if (DestinationMapAssetName != FString(""))
	{
		SaveObject->MapAssetName = DestinationMapAssetName;
		SaveObject->MapName = GetMapNameFromMapAssetName(DestinationMapAssetName);
	}

	if (!SaveObject->HasMap(WorldName))
	{
		FSavedMap NewSavedMap;
		NewSavedMap.MapAssetName = WorldName;
		SaveObject->SavedMaps.Add(NewSavedMap);
	}

	// 맵의 액터들 저장
	FSavedMap SavedMap = SaveObject->GetSavedMapWithMapName(WorldName);
	SavedMap.SavedActors.Empty();

	for (FActorIterator It(World); It; ++It)
	{
		AActor* Actor = *It;
		if(!IsValid(Actor) || !Actor->Implements<USaveInterface>()) continue;

		FSavedActor SavedActor;
		SavedActor.ActorName = Actor->GetFName();
		SavedActor.Transform = Actor->GetTransform();

		// 메모리에 데이터를 직렬화하기 위한 클래스
		FMemoryWriter MemoryWriter(SavedActor.Bytes);
		// UObject 객체를 이름 기준으로 직렬화해주는 클래스
		FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);
		Archive.ArIsSaveGame = true;

		Actor->Serialize(Archive);

		SavedMap.SavedActors.AddUnique(SavedActor);
	}

	for (FSavedMap& MapToReplace : SaveObject->SavedMaps)
	{
		if (MapToReplace.MapAssetName == WorldName)
		{
			MapToReplace = SavedMap;
			break;
		}
	}

	UGameplayStatics::SaveGameToSlot(SaveObject, AuraGameInstance->GetLoadSlotName(), AuraGameInstance->GetLoadSlotIndex());
}

void AAuraGameModeBase::LoadWorldState(UWorld* World) const
{
	if (!World) return;

	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	if (!AuraGameInstance) return;

	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	if (UGameplayStatics::DoesSaveGameExist(AuraGameInstance->GetLoadSlotName(), AuraGameInstance->GetLoadSlotIndex()))
	{
		ULoadScreenSaveGame* SaveObject = Cast<ULoadScreenSaveGame>(UGameplayStatics::LoadGameFromSlot(AuraGameInstance->GetLoadSlotName(), AuraGameInstance->GetLoadSlotIndex()));
		if (!SaveObject)
		{
			UE_LOG(LogAura, Error, TEXT("Failed to load slot"));
			return;
		}

		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (!IsValid(Actor) || !Actor->Implements<USaveInterface>()) continue;

			for (const FSavedActor& SavedActor : SaveObject->GetSavedMapWithMapName(WorldName).SavedActors)
			{
				if (SavedActor.ActorName == Actor->GetFName())
				{
					if (ISaveInterface::Execute_ShouldLoadTransform(Actor))
					{
						Actor->SetActorTransform(SavedActor.Transform);
					}

					FMemoryReader MemoryReader(SavedActor.Bytes);
					FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
					Archive.ArIsSaveGame = true;

					// 저장되어있던 직렬화 데이터를 역직렬화해서 액터에 세팅
					Actor->Serialize(Archive);

					// 로드되었으므로 체크포인트에 불을 킨다
					ISaveInterface::Execute_LoadActor(Actor);
				}
			}
		}
	}
}

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	Maps.Add(DefaultMapName, DefaultMap);
}

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	if (!AuraGameInstance) return nullptr;

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);

	if (!Actors.IsEmpty())
	{
		// 기본값은 첫번째 객체 리턴
		AActor* SelectedActor = Actors[0];
		for (AActor* Actor : Actors)
		{
			if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
			{
				// 특정 태그를 가진 플레이어 스타트 객체를 찾음
				if (PlayerStart->PlayerStartTag == AuraGameInstance->GetPlayerStartTag())
				{
					SelectedActor = PlayerStart;
					break;
				}
			}
		}
		return SelectedActor;
	}
	return nullptr;
}

FString AAuraGameModeBase::GetMapNameFromMapAssetName(const FString& MapAssetName) const
{
	for (const TPair<FString, TSoftObjectPtr<UWorld>>& Map : Maps)
	{
		if (Map.Value.ToSoftObjectPath().GetAssetName() == MapAssetName)
		{
			return Map.Key;
		}
	}
	return FString();
}
