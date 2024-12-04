// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadScreen.generated.h"

class UMVVM_LoadSlot;

/**
 * ·Îµå ¸Þ´º¿Í ¿¬°áµÈ ºä¸ðµ¨ Å¬·¡½º
 */
UCLASS()
class AURA_API UMVVM_LoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	// ·Îµå ½½·Ô ºä¸ðµ¨µé »ý¼º
	void InitializeLoadSlot();

	// ·Îµå ½½·Ô °¡Á®¿À±â
	UFUNCTION(BlueprintPure)
	UMVVM_LoadSlot* GetLoadSlotViewModelByIndex(int32 Index) const;

private:
	// ·Îµå ½½·Ô ºä¸ðµ¨ Å¬·¡½º
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadSlot> LoadSlotViewModelClass;

	// ·Îµå ½½·Ôµé
	UPROPERTY()
	TMap<int32, UMVVM_LoadSlot*> LoadSlots;

	// °¢°¢ÀÇ ·Îµå ½½·Ô
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_0;
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_1;
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_2;
};
