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

	// ���̺� ������ �ߺ��� ��� ���� �� ����
	DeleteSlot(LoadSlot->GetSlotName(), SlotIndex);

	// ������ ����
	LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
	LoadScreenSaveGame->MapName = LoadSlot->GetMapName();
	LoadScreenSaveGame->SaveSlotStatus = ESaveSlotStatus::Taken;
	LoadScreenSaveGame->PlayerStartTag = LoadSlot->GetPlayerStartTag();

	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->GetSlotName(), SlotIndex);
}

ULoadScreenSaveGame* AAuraGameModeBase::GetSaveLoadData(const FString& SlotName, int32 SlotIndex) const
{
	USaveGame* SaveGameObject = nullptr;

	// ���Կ� ����Ǿ������� �װ� ����, �ƴϸ� ���� �����.
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

	// ���� �ν��Ͻ����� �ֽ� ���� ����
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

	// �� ����
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	// ���� ������ �� ������ �ִٸ� ����
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

	// ���� ���͵� ����
	FSavedMap SavedMap = SaveObject->GetSavedMapWithMapName(WorldName);
	SavedMap.SavedActors.Empty();

	for (FActorIterator It(World); It; ++It)
	{
		AActor* Actor = *It;
		if(!IsValid(Actor) || !Actor->Implements<USaveInterface>()) continue;

		FSavedActor SavedActor;
		SavedActor.ActorName = Actor->GetFName();
		SavedActor.Transform = Actor->GetTransform();

		// �޸𸮿� �����͸� ����ȭ�ϱ� ���� Ŭ����
		FMemoryWriter MemoryWriter(SavedActor.Bytes);
		// UObject ��ü�� �̸� �������� ����ȭ���ִ� Ŭ����
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

					// ����Ǿ��ִ� ����ȭ �����͸� ������ȭ�ؼ� ���Ϳ� ����
					Actor->Serialize(Archive);

					// �ε�Ǿ����Ƿ� üũ����Ʈ�� ���� Ų��
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
		// �⺻���� ù��° ��ü ����
		AActor* SelectedActor = Actors[0];
		for (AActor* Actor : Actors)
		{
			if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
			{
				// Ư�� �±׸� ���� �÷��̾� ��ŸƮ ��ü�� ã��
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
