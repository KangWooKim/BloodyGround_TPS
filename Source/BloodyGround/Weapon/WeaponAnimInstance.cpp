// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponAnimInstance.h"
#include "BaseWeapon.h"

void UWeaponAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ABaseWeapon* Weapon = Cast<ABaseWeapon>(TryGetPawnOwner());

	if (Weapon)
	{
		WeaponState = Weapon->GetWeaponState();
	}
}
