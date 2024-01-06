#include "InGameWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UInGameWidget::UpdateHealthBar(float HealthPercentage)
{
    if (HealthBar)
    {
        HealthBar->SetPercent(HealthPercentage);
    }
}

void UInGameWidget::UpdateAmmoCount(int32 AmmoInMagazine, int32 TotalAmmo)
{
    if (AmmoText)
    {
        FString AmmoDisplay = FString::Printf(TEXT("%d / %d"), AmmoInMagazine, TotalAmmo - AmmoInMagazine);
        AmmoText->SetText(FText::FromString(AmmoDisplay));
    }
}
