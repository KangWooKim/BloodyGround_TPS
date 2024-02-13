// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BloodyGroundGameState.generated.h"

UENUM(BlueprintType)
enum class EDayNightCycle : uint8
{
	Day UMETA(DisplayName = "Day"),
	Night UMETA(DisplayName = "Night")
};

UCLASS()
class BLOODYGROUND_API ABloodyGroundGameState : public AGameState
{
	GENERATED_BODY()
	
public:

    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

    // 블루프린트에서 접근 및 수정 가능하도록 설정
    UPROPERTY(ReplicatedUsing = OnRep_DayNightChanged, BlueprintReadWrite, EditDefaultsOnly, Category = "DayNight")
        EDayNightCycle DayNightCycle;

    // 낮/밤 상태 변경시 호출되는 함수
    UFUNCTION()
        void OnRep_DayNightChanged();

    // 낮/밤 상태를 설정하는 함수
    void SetDayNightCycle(EDayNightCycle NewCycle);
};
