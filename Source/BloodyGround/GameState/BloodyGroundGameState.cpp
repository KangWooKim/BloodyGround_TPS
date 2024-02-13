// Fill out your copyright notice in the Description page of Project Settings.


#include "BloodyGroundGameState.h"
#include "Net/UnrealNetwork.h"

void ABloodyGroundGameState::SetDayNightCycle(EDayNightCycle NewCycle)
{
    if (HasAuthority())
    {
        DayNightCycle = NewCycle;
    }
}

void ABloodyGroundGameState::OnRep_DayNightChanged()
{
    // 낮/밤 상태가 변경되었을 때 필요한 로직을 여기에 구현합니다.
    // 예: 낮과 밤에 따라 환경 조명 변경, 좀비 AI 상태 변경 등
}

void ABloodyGroundGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 낮/밤 상태 변수 복제
    DOREPLIFETIME(ABloodyGroundGameState, DayNightCycle);
}