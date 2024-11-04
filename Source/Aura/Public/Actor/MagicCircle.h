// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MagicCircle.generated.h"

/**
 * ������ Ŭ����
 */
UCLASS()
class AURA_API AMagicCircle : public AActor
{
	GENERATED_BODY()
	
private:
	AMagicCircle();

private:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UDecalComponent* GetMagicCircleDecal() const { return MagicCircleDecal; }

protected:
	// �������� ǥ���� ��Į ����Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UDecalComponent> MagicCircleDecal;
};
