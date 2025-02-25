// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ViewModel/MVVM_LoadScreen.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "Game/AuraGameModeBase.h"
#include "Game/AuraGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UMVVM_LoadScreen::SetNumLoadSlot(int32 InNumLoadSlot)
{
	UE_MVVM_SET_PROPERTY_VALUE(NumLoadSlot, InNumLoadSlot);
}

void UMVVM_LoadScreen::InitializeLoadSlot()
{
	if (!LoadSlotViewModelClass) return;

	LoadSlots.Reserve(3);
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_0->SetSlotName(TEXT("LoadSlot_0"));
	LoadSlot_0->SetSlotIndex(0);
	LoadSlots.Add(0, LoadSlot_0);

	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1->SetSlotName(TEXT("LoadSlot_1"));
	LoadSlot_1->SetSlotIndex(1);
	LoadSlots.Add(1, LoadSlot_1);

	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2->SetSlotName(TEXT("LoadSlot_2"));
	LoadSlot_2->SetSlotIndex(2);
	LoadSlots.Add(2, LoadSlot_2);

	SetNumLoadSlot(LoadSlots.Num());
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return LoadSlots.FindChecked(Index);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FString& EnteredName)
{
	if (AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		LoadSlots[Slot]->SetPlayerName(EnteredName);
		LoadSlots[Slot]->SetMapName(AuraGameMode->GetDefaultMapName());
		LoadSlots[Slot]->SetPlayerLevel(1);
		LoadSlots[Slot]->SetSlotStatus(ESaveSlotStatus::Taken);
		LoadSlots[Slot]->SetPlayerStartTag(AuraGameMode->GetDefaultPlayerStartTag());
		LoadSlots[Slot]->SetMapAssetName(AuraGameMode->GetDefaultMap().ToSoftObjectPath().GetAssetName());
		AuraGameMode->SaveSlotData(LoadSlots[Slot], Slot);
		LoadSlots[Slot]->InitializeSlot();

		// 게임 인스턴스에 마지막에 추가한 슬롯의 정보를 기입
		if (UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(AuraGameMode->GetGameInstance()))
		{
			AuraGameInstance->SetPlayerStartTag(AuraGameMode->GetDefaultPlayerStartTag());
			AuraGameInstance->SetLoadSlotName(LoadSlots[Slot]->GetSlotName());
			AuraGameInstance->SetLoadSlotIndex(LoadSlots[Slot]->GetSlotIndex());
		}
	}
}

void UMVVM_LoadScreen::NewGameButtonPressed(int32 Slot)
{
	LoadSlots[Slot]->SetWidgetSwitcherIndexDelegate.Broadcast(1);
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 Slot)
{
	SlotSelectedDelegate.Broadcast();

	for (const TTuple<int32, UMVVM_LoadSlot*>& LoadSlot : LoadSlots)
	{
		if (LoadSlot.Value)
		{
			// 선택한 슬롯은 버튼을 비활성화
			if (LoadSlot.Key == Slot)
			{
				LoadSlot.Value->EnableSelectSlotButtonDelegate.Broadcast(false);
			}
			// 나머지는 활성화
			else
			{
				LoadSlot.Value->EnableSelectSlotButtonDelegate.Broadcast(true);
			}
		}
	}

	SelectedSlot = LoadSlots[Slot];
}

void UMVVM_LoadScreen::DeleteButtonPressed()
{
	if (!IsValid(SelectedSlot)) return;

	AAuraGameModeBase::DeleteSlot(SelectedSlot->GetSlotName(), SelectedSlot->GetSlotIndex());
	SelectedSlot->SetSlotStatus(ESaveSlotStatus::Vacant);
	// 슬롯 수정사항 업데이트
	SelectedSlot->InitializeSlot();
	// 선택 버튼은 다시 활성화
	SelectedSlot->EnableSelectSlotButtonDelegate.Broadcast(true);
}

void UMVVM_LoadScreen::PlayButtonPressed()
{
	if (!IsValid(SelectedSlot)) return;

	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (!AuraGameMode) return;

	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(AuraGameMode->GetGameInstance());
	if (!AuraGameInstance) return;

	// 마지막으로 선택한 슬롯의 플레이어 스타트 태그를 설정
	AuraGameInstance->SetPlayerStartTag(SelectedSlot->GetPlayerStartTag());
	AuraGameInstance->SetLoadSlotName(SelectedSlot->GetSlotName());
	AuraGameInstance->SetLoadSlotIndex(SelectedSlot->GetSlotIndex());

	// 해당 맵으로 이동
	AuraGameMode->TravelToMap(SelectedSlot);
}

void UMVVM_LoadScreen::LoadData()
{
	if (AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		for (const TTuple<int32, UMVVM_LoadSlot*>& LoadSlot : LoadSlots)
		{
			if (LoadSlot.Value)
			{
				// 각가 슬롯에 저장되어있는 세이브 게임 오브젝트를 가져와서 뷰모델에 세팅
				if (ULoadScreenSaveGame* SaveObject = AuraGameMode->GetSaveLoadData(LoadSlot.Value->GetSlotName(), LoadSlot.Key))
				{
					LoadSlot.Value->SetPlayerName(SaveObject->PlayerName);
					LoadSlot.Value->SetMapName(SaveObject->MapName);
					LoadSlot.Value->SetPlayerLevel(SaveObject->PlayerLevel);
					LoadSlot.Value->SetSlotStatus(SaveObject->SaveSlotStatus);
					LoadSlot.Value->SetPlayerStartTag(SaveObject->PlayerStartTag);
					LoadSlot.Value->InitializeSlot();
				}
			}
		}
	}
}
