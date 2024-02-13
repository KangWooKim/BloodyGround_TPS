#include "MachineGun.h"
#include "BloodyGround/Component/InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "BloodyGround/Character/BaseCharacter.h"
#include "BloodyGround/HUD/InGameHUD.h"

AMachineGun::AMachineGun()
{
	Capacity = 30;
	CurrentAmmo = Capacity;
	Damage = 30;

    FireRate = 0.1f; // 0.1초 간격으로 발사
}

void AMachineGun::Fire()
{
    if (CanFire()) {

        return;
    }

	Super::Fire();
    CurrentAmmo--;
    if (Character->GetInGameHUD())
    {
        Character->GetInGameHUD()->UpdateAmmo(CurrentAmmo, Character->InventoryComp->GetMachineGunAmmo());
    }
}

void AMachineGun::FireEnd()
{
    Super::FireEnd();
    GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

void AMachineGun::ShootEnd()
{
    WeaponState = EWeaponState::None;
    GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AMachineGun::Fire, FireRate, false, FireRate);
}

bool AMachineGun::CanFire()
{
    return Character == nullptr || Character->InventoryComp == nullptr || Character->InventoryComp->GetMachineGunAmmo() < 1 || CurrentAmmo < 1 || WeaponState == EWeaponState::Reload;
}

EWeaponType AMachineGun::GetCurrentWeaponType()
{
    return EWeaponType::MachineGun;
}

void AMachineGun::PerformReload()
{
    Super::PerformReload();

    if (Character == nullptr || Character->InventoryComp == nullptr || WeaponState != EWeaponState::None)
    {
        return; // 캐릭터 또는 인벤토리 컴포넌트가 없는 경우 리턴
    }

    int32 AmmoToReload = FMath::Min(Capacity - CurrentAmmo, Character->InventoryComp->GetMachineGunAmmo());
    CurrentAmmo += AmmoToReload;
    Character->InventoryComp->SetMachineGunAmmo(Character->InventoryComp->GetMachineGunAmmo() - AmmoToReload);

    if (Character->GetInGameHUD())
    {
        Character->GetInGameHUD()->UpdateAmmo(CurrentAmmo, Character->InventoryComp->GetMachineGunAmmo());
    }
}
