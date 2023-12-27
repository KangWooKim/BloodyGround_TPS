#include "BaseZombieAnimInstance.h"
#include "BaseZombie.h"

void UBaseZombieAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    // 이동 속도 업데이트
    ABaseZombie* Zombie = Cast<ABaseZombie>(TryGetPawnOwner());
    if (Zombie)
    {
        MovementSpeed = Zombie->GetVelocity().Size();

        ZombieState = Zombie->GetZombieState();
    }
}
