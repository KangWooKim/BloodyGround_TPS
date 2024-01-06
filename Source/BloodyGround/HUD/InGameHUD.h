// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "InGameHUD.generated.h"

class UInGameWidget;

/**
 * 
 */
UCLASS()
class BLOODYGROUND_API AInGameHUD : public AHUD
{
	GENERATED_BODY()

public:

	FORCEINLINE UInGameWidget* GetInGameWidget() { return InGameWidget; }

protected:


private:

	UPROPERTY()
	UInGameWidget* InGameWidget;
	
};
