#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Pistol.generated.h"

UCLASS()
class BLOODYGROUND_API APistol : public ABaseWeapon
{
	GENERATED_BODY()

public:
	// Pistol 클래스 생성자
	APistol();

	UFUNCTION()
	virtual void Fire() override;

	virtual EWeaponType GetCurrentWeaponType() override;

	virtual void PerformReload() override;

protected:

	UFUNCTION()
	virtual bool CanFire() override;

private:
	
	
};
