#include "AuraAbilityTypes.h"

FAuraGameplayEffectContext::FAuraGameplayEffectContext()
	: bIsBlockedHit(false)
	, bIsCriticalHit(false)
{
}

UScriptStruct* FAuraGameplayEffectContext::GetScriptStruct() const
{
	return nullptr;
}

bool FAuraGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	return false;
}
