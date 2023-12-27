// Fill out your copyright notice in the Description page of Project Settings.


#include "BloodyGroundPlayerController.h"
#include "GameFramework/PlayerState.h"


ABloodyGroundPlayerController::ABloodyGroundPlayerController()
{
    // RTT 초기화
    RoundTripTime = 0.0f;
    LastRTTUpdateTime = 0.0f;
}

void ABloodyGroundPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);

    // RTT 갱신
    if (GetWorld()->TimeSince(LastRTTUpdateTime) > 1.0f)  // 예: 매 1초마다 갱신
    {
        if (PlayerState && IsLocalController())
        {
            RoundTripTime = PlayerState->GetPing() * 0.001f;  // 밀리초를 초로 변환
            LastRTTUpdateTime = GetWorld()->GetTimeSeconds();
        }
    }
}