// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseWeapon.h"
#include "WeaponAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BLOODYGROUND_API UWeaponAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	// 애니메이션 업데이트 시 호출되는 함수
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WeaponState")
	EWeaponState WeaponState;

private:


};
