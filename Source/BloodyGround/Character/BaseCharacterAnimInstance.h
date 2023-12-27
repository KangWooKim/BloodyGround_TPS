#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseCharacter.h"
#include "BloodyGround/Weapon/BaseWeapon.h"
#include "BaseCharacterAnimInstance.generated.h"

UCLASS()
class BLOODYGROUND_API UBaseCharacterAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    // 캐릭터의 지면 속도
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
        float GroundSpeed;

    // 캐릭터의 전방 이동량
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
        float ForwardMovement;

    // 캐릭터의 좌우 이동량
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
        float RightMovement;

    // 캐릭터가 공중에 있는지 여부
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
        bool bIsInAir;

    // 캐릭터가 조준하고 있는지 여부
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action")
        bool bIsAiming;

    // 현재 캐릭터가 들고 있는 무기의 타입
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
        EWeaponType CurrentWeaponType;

    // 무기 변경 애니메이션 재생 함수
    UFUNCTION()
        void PlayWeaponSwitchAnimation();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action")
    FVector LeftHandSocketLocation;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Action")
        ECharacterState CharacterState;

protected:
    // 애니메이션 업데이트 시 호출되는 함수
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};