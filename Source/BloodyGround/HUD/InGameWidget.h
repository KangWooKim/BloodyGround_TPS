#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class BLOODYGROUND_API UInGameWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "HUD")
        void UpdateHealthBar(float HealthPercentage);

    UFUNCTION(BlueprintCallable, Category = "HUD")
        void UpdateAmmoCount(int32 AmmoInMagazine, int32 TotalAmmo);

protected:
    // ����� ǥ�� ���� ����
    UPROPERTY(meta = (BindWidget))
        UProgressBar* HealthBar;

    // ź�� �� ǥ�� �ؽ�Ʈ ����
    UPROPERTY(meta = (BindWidget))
        UTextBlock* AmmoText;
};
