// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameHUD.h"
#include "BloodyGround/HUD/InGameWidget.h"

void AInGameHUD::UpdateHealth(float HealthPercentage)
{
	InGameWidget->UpdateHealthBar(HealthPercentage);
}

void AInGameHUD::UpdateAmmo(int32 AmmoInMagazine, int32 TotalAmmo)
{
	InGameWidget->UpdateAmmoCount(AmmoInMagazine, TotalAmmo);
}

void AInGameHUD::BeginPlay()
{
	Super::BeginPlay();

	InGameWidget = Cast<UInGameWidget>(InGameWidget);
}
