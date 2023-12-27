#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseZombie.h"
#include "BaseZombieAnimInstance.generated.h"

UCLASS()
class BLOODYGROUND_API UBaseZombieAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    // 애니메이션 업데이트 시 호출되는 함수
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
    // 좀비의 이동 속도
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
        float MovementSpeed;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
     EZombieState ZombieState;
};
