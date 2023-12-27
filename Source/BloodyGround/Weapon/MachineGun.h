// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "MachineGun.generated.h"

/**
 * 
 */
UCLASS()
class BLOODYGROUND_API AMachineGun : public ABaseWeapon
{
	GENERATED_BODY()
	
public:

	AMachineGun();

	UFUNCTION()
	virtual void Fire() override;

	virtual EWeaponType GetCurrentWeaponType() override;

protected:

	UFUNCTION()
	virtual bool CanFire() override;

	virtual void FireEnd() override;

	UFUNCTION(BluePrintCallable)
	void ShootEnd();

private:

	FTimerHandle FireTimerHandle; // 연속 발사를 위한 타이머 핸들
	float FireRate;               // 발사 간격

};
