#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UInventoryComponent::AddWeapon(ABaseWeapon* NewWeapon)
{
    if (NewWeapon && !Weapons.Contains(NewWeapon))
    {
        Weapons.Add(NewWeapon);
    }
}

void UInventoryComponent::RemoveWeapon(ABaseWeapon* WeaponToRemove)
{
    if (WeaponToRemove && Weapons.Contains(WeaponToRemove))
    {
        Weapons.Remove(WeaponToRemove);
    }
}

void UInventoryComponent::SetInitWeapon(ABaseWeapon* Weapon)
{
    CurrentWeapon = Weapon;
}

void UInventoryComponent::ChangeWeapon()
{
    if (Weapons.Num() <= 1)
    {
        // 무기가 한 개 이하일 경우 전환할 수 없음
        return;
    }

    // 현재 무기 인덱스 찾기
    int32 CurrentWeaponIndex = Weapons.IndexOfByKey(CurrentWeapon);
    int32 NextWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num(); // 다음 무기 인덱스 계산

    ABaseWeapon* NewWeapon = Weapons[NextWeaponIndex];

    if (CurrentWeapon)
    {
        CurrentWeapon->WeaponState = EWeaponState::None;
        CurrentWeapon->SetActorHiddenInGame(true); // 현재 무기 숨기기
    }

    CurrentWeapon = NewWeapon; // 현재 무기 업데이트
    CurrentWeapon->SetActorHiddenInGame(false); // 새 무기 보이기

    // 무기 전환에 관련된 추가 로직 (예: 애니메이션, 사운드 등)
}

ABaseWeapon* UInventoryComponent::GetCurrentWeapon() const
{
    return CurrentWeapon;
}

void UInventoryComponent::SetPistolAmmo(int32 NewAmmo)
{
    PistolAmmo = NewAmmo;
}

void UInventoryComponent::SetMachineGunAmmo(int32 NewAmmo)
{
    MachineGunAmmo = NewAmmo;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UInventoryComponent, Weapons);
    DOREPLIFETIME(UInventoryComponent, CurrentWeapon);
    DOREPLIFETIME(UInventoryComponent, PistolAmmo);
    DOREPLIFETIME(UInventoryComponent, MachineGunAmmo);
}