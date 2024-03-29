#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Animation/AnimMontage.h"
#include "BaseWeapon.generated.h"


UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None    UMETA(DisplayName = "None"),
	Pistol  UMETA(DisplayName = "Pistol"),
	MachineGun UMETA(DisplayName = "MachineGun")
};

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
    None    UMETA(DisplayName = "None"),
    Fire  UMETA(DisplayName = "Fire"),
    Reload  UMETA(DisplayName = "Reload")
};

UCLASS()
class BLOODYGROUND_API ABaseWeapon : public APawn
{
    GENERATED_BODY()

public:
    ABaseWeapon();

    UFUNCTION(NetMulticast, Reliable)
    // 클라이언트에서 발사 애니메이션을 재생하기 위한 함수
    void MulticastPlayFireAnimation();

    UFUNCTION(Server, Reliable)
    void ServerPlayFireAnimation();

protected:
    virtual void BeginPlay() override;

    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* SkeletalMeshComponent;

    UPROPERTY()
    class ABaseCharacter* Character;

    UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = Noise)
    class UPawnNoiseEmitterComponent* NoiseEmitter;

     virtual bool CanFire();

public:
    virtual void Tick(float DeltaTime) override;

    // 무기 발사 함수
    virtual void Fire();

    // 탄창 리로드 함수
    UFUNCTION(BlueprintCallable)
    void Reload();

    UFUNCTION()
     virtual void PerformReload();

    // 서버에서 재장전을 처리하기 위한 함수
    UFUNCTION(Server, Reliable)
     void ServerReload();

    UFUNCTION(NetMulticast, Reliable)
    void MulticastPlayReloadAnimation();

    virtual EWeaponType GetCurrentWeaponType();

    // 탄창 내 현재 탄알 수
    UPROPERTY(Replicated ,EditDefaultsOnly, Category = "Weapon")
    int32 CurrentAmmo;

    // 탄창 최대 용량
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    int32 Capacity;

    // 데미지 저장 변수
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    int32 Damage;

    UFUNCTION(BlueprintCallable)
     virtual void FireEnd();

    UFUNCTION(BlueprintCallable)
     void ReloadEnd();

    FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() {return SkeletalMeshComponent;}
    FORCEINLINE EWeaponState GetWeaponState() { return WeaponState; }

    UPROPERTY(Replicated)
    EWeaponState WeaponState;

    // 발사 애니메이션 몽타주
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    class UAnimMontage* FireAnimation;

    // 재장전 애니메이션 몽타주
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    class UAnimMontage* ReloadAnimation;

    UFUNCTION(Server, Reliable)
    // 소음을 발생시키는 함수
    void WeaponNoise(float Loudness);

protected:

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerCheckHit(FHitResult HitResult, float HitTime, FVector StartLocation, FVector EndDirection);

   
private:

    UPROPERTY()
    float HitThreshold;
};