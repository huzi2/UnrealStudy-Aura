// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

// 적용된 이펙트 자체의 에셋 태그를 확인해서 해당 태그에 대한 델리게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer&)
// 어빌리티가 적용 때마다 수행할 델리게이트
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven);
// 어빌리티에 필요한 정보를 넣기위해 오버레이위젯컨트롤러에서 연결하고 ForEachAbility()에서 수행할 델리게이트
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);
// 어빌리티 상태가 변경되었을 때 사용할 델리게이트. 어빌리티 태그와 상태 태그, 어빌리티 레벨
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged, const FGameplayTag&, const FGameplayTag&, int32);

/**
 * 커스텀 어빌리티 시스템 컴포넌트
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

private:
	virtual void OnRep_ActivateAbilities() final;

public:
	// 어빌리티에서 어빌리티 태그 얻어오기
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	// 어빌리티에서 인풋 태그 얻어오기
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	// 어빌리티에서 어빌리티의 상태 태그 얻어오기
	static FGameplayTag GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec);

public:
	// 어빌리티 태그에서 통해 어빌리티 얻어오기
	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);

	// 어빌리티 태그에서 어빌리티 설명 얻어오기
	bool GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription);

	// 이펙트 적용될 때 함수에 바인드
	void AbilityActorInfoSet();

	// 캐릭터에 어빌리티 추가
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilites);
	// 캐릭터에 패시브 어빌리티 추가
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilites);

	// 입력이 릴리즈되었을 때 연결된 태그 함수 호출
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	// 입력이 눌러졌을 때 연결된 태그 함수 호출
	void AbilityInputTagHeld(const FGameplayTag& InputTag);

	// 연결된 모든 어빌리티 수행
	void ForEachAbility(const FForEachAbility& Delegate);

	// 능력치 상승
	void UpgradeAttribute(const FGameplayTag& AttributeTag);

	// 레벨업할 때 어빌리티 상태 설정
	void UpdateAbilityStatuses(int32 Level);

	// 서버한테 스킬 포인트 사용 요청
	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag);

private:
	// 서버한테 능력치 상승 요청
	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);

	// OnGameplayEffectAppliedDelegateToSelf 델리게이트가 서버에서만 호출된다(엔진내용). 그래서 아래 함수를 Client RPC로 만들어 클라이언트에서도 호출되도록 한다.
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

	// 클라이언트에게 어빌리티 상태가 변경되었음을 알림
	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel);

public:
	// 이펙트에 적용된 태그와 연동할 델리게이트
	FEffectAssetTags EffectAssetTagsDelegate;
	// 어빌리티가 적용될 때 델리게이트
	FAbilitiesGiven AbilitiesGivenDelegate;
	// 어빌리티 상태가 수정될 때 사용할 델리게이트
	FAbilityStatusChanged AbilityStatusChangedDelegate;
	// 시작 어빌리티들을 적용시켰는 지 확인
	bool bStartupAbilitiesGiven = false;
};
