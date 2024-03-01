// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD/AuraHUD.h"
#include "Player/AuraPlayerState.h"
#include "UI/WidgetController/AuraWidgetController.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			if (AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>())
			{
				if (UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent())
				{
					if (UAttributeSet* AS = PS->GetAttributeSet())
					{
						const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
						return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);
					}
				}
			}
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			if (AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>())
			{
				if (UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent())
				{
					if (UAttributeSet* AS = PS->GetAttributeSet())
					{
						const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
						return AuraHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
					}
				}
			}
		}
	}
	return nullptr;
}
