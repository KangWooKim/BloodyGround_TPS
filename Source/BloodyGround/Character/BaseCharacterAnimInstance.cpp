#include "BaseCharacterAnimInstance.h"
#include "BaseCharacter.h"
#include "BloodyGround/Component/InventoryComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    ABaseCharacter* BaseChar = Cast<ABaseCharacter>(TryGetPawnOwner());
    if (BaseChar)
    {
        FVector Velocity = BaseChar->GetVelocity();
        GroundSpeed = Velocity.Size();

        // 캐릭터의 전방과 우측 벡터
        FVector ForwardVector = BaseChar->GetActorForwardVector();
        FVector RightVector = BaseChar->GetActorRightVector();

        CharacterState = BaseChar->GetCharacterState();

        // 전방 및 좌우 속도 계산
        float ForwardSpeed = FVector::DotProduct(Velocity, ForwardVector);
        float RightSpeed = FVector::DotProduct(Velocity, RightVector);

        // 애니메이션 블루프린트 변수 업데이트
        bIsInAir = BaseChar->GetCharacterMovement()->IsFalling();
        bIsAiming = BaseChar->BattleComp ? BaseChar->BattleComp->bIsAiming : false;
        ForwardMovement = ForwardSpeed;
        RightMovement = RightSpeed;

        ABaseWeapon* CurrentWeapon = BaseChar->InventoryComp->GetCurrentWeapon();
        if (CurrentWeapon)
        {
            CurrentWeaponType = CurrentWeapon->GetCurrentWeaponType();
            LeftHandSocketLocation = CurrentWeapon->GetWeaponMesh()->GetSocketLocation(TEXT("LeftHandSocket"));
        }

        
    }
}

void UBaseCharacterAnimInstance::PlayWeaponSwitchAnimation()
{
    // 무기 변경 애니메이션 선택 및 재생 로직
    // 예: Pistol -> MachineGun 변경 시, Pistol을 숨기고 MachineGun 뽑기 애니메이션 재생
}
