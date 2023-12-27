// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BloodyGroundPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLOODYGROUND_API ABloodyGroundPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ABloodyGroundPlayerController();

    FORCEINLINE float GetRoundTripTime() { return RoundTripTime; }

protected:
    virtual void PlayerTick(float DeltaTime) override;

private:
    // RTT 관련 변수
    UPROPERTY(VisibleAnywhere, Category = "Network")
        float RoundTripTime;

    // 마지막으로 RTT를 갱신한 시점
    float LastRTTUpdateTime;
};