#include "Pistol.h"
#include "BloodyGround/Component/InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "BloodyGround/Character/BaseCharacter.h"

APistol::APistol()
{
    Capacity = 10;
    CurrentAmmo = Capacity;
    Damage = 10;

}

void APistol::Fire()
{
    if (CanFire()) return;
   
    Super::Fire();

    Character->InventoryComp->UsePistolAmmo();
    CurrentAmmo--;
}

bool APistol::CanFire()
{
    return WeaponState == EWeaponState::Reload || Character == nullptr || Character->InventoryComp == nullptr || Character->InventoryComp->GetPistolAmmo() < 1 || CurrentAmmo < 1;
}

EWeaponType APistol::GetCurrentWeaponType()
{
    return EWeaponType::Pistol;
}