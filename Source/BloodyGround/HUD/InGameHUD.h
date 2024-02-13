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

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> InGameWidgetClass;

	UFUNCTION()
	void UpdateHealth(float HealthPercentage);

	UFUNCTION()
	void UpdateAmmo(int32 AmmoInMagazine, int32 TotalAmmo);

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY()
	class UInGameWidget* InGameWidget;
	
};
