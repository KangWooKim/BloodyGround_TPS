#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BloodyGround/Weapon/BaseWeapon.h"
#include "BloodyGround/Weapon/Pistol.h"
#include "BloodyGround/Weapon/MachineGun.h"
#include "InventoryComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BLOODYGROUND_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(Replicated)
        TArray<ABaseWeapon*> Weapons; // 플레이어가 소지한 무기들의 목록

    UPROPERTY(Replicated)
        ABaseWeapon* CurrentWeapon; // 현재 활성화된 무기

    // 탄알 수 저장
    UPROPERTY(Replicated)
        int32 PistolAmmo;

    UPROPERTY(Replicated)
        int32 MachineGunAmmo;

public:
    // 무기 추가 및 제거
    void AddWeapon(ABaseWeapon* NewWeapon);
    void RemoveWeapon(ABaseWeapon* WeaponToRemove);
    void SetInitWeapon(ABaseWeapon* Weapon);

    // 무기 전환
    void ChangeWeapon();

    // 현재 무기 가져오기
    ABaseWeapon* GetCurrentWeapon() const;

    // 탄알 관련 함수
    void SetPistolAmmo(int32 NewAmmo);
    int32 GetPistolAmmo() const;
    void SetMachineGunAmmo(int32 NewAmmo);
    int32 GetMachineGunAmmo() const;

    void UsePistolAmmo();
    void UseMachineGunAmmo();

    // 네트워크 복제를 위한 함수
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};