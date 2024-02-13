// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BloodyGroundGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class BLOODYGROUND_API ABloodyGroundGameModeBase : public AGameMode
{
	GENERATED_BODY()
	
public:

	ABloodyGroundGameModeBase();

	// 플레이어를 리스폰하는 함수 선언
	void RespawnPlayer(APlayerController* PC);
};
