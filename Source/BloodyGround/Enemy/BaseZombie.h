#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/PawnSensingComponent.h"
#include "BloodyGround/Component/ServerLocationComponent.h"
#include "BaseZombie.generated.h"

UENUM(BlueprintType)
enum class EZombieState : uint8
{
    None    UMETA(DisplayName = "None"),
    Attacking  UMETA(DisplayName = "Attacking"),
    HitReact  UMETA(DisplayName = "HitReact"),
    Death  UMETA(DisplayName = "Death"),
    Sleep  UMETA(DiaPlayName = "Sleep"),
    MAX UMETA(DisplayName = "MAX")
};


UCLASS()
class BLOODYGROUND_API ABaseZombie : public ACharacter
{
    GENERATED_BODY()

public:
    ABaseZombie();

    UFUNCTION()
    virtual void TakeShot(FHitResultData HitResult, float WeaponDamage);

    FORCEINLINE EZombieState GetZombieState() { return ZombieState; }

    FORCEINLINE UServerLocationComponent* GetServerLocationComponent() { return ServerLocationComp; }

protected:

    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
    virtual void PostInitializeComponents() override;

    // 피격 및 사망 처리 관련
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
    void HandleDeath();
    virtual void Tick(float DeltaTime) override;

    UFUNCTION()
        void OnSeePlayer(APawn* Pawn);

    UFUNCTION()
    bool IsTargetInSight(APawn* Target);


    UFUNCTION()
        void RestartPatrol();

    UFUNCTION()
        void OnHearNoise(APawn* NoiseInstigator, const FVector& Location, float Volume);

    UFUNCTION()
    virtual void Attack(APawn* Target);

    UFUNCTION(BlueprintCallable, Category = "Zombie|Combat")
     virtual  void ApplyDamageToTarget();

    UFUNCTION(BlueprintCallable, Category = "Zombie|Combat")
        void HitReactEnd();

    UFUNCTION(BlueprintCallable, Category = "Zombie|Combat")
        void DeathEnd();

    UPROPERTY(EditAnywhere)
    float Damage;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Zombie|Health")
        float Health;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Combat")
        float AttackRange;  // 공격가능범위

    UPROPERTY(EditDefaultsOnly, Category = "Zombie|AI")
        float PatrolRadius;

    UPROPERTY(EditDefaultsOnly, Category = "Zombie|AI")
        float TimeToSleep;

    UPROPERTY(Replicated)
     // 현재 추적 중인 타겟
     APawn* CurrentTarget;

    UPROPERTY(Replicated)
     EZombieState ZombieState;

    // 공격 대기 시간 (초)
    float AttackCooldown = 5.0f;

    FTimerHandle TimerHandle_HitReactEnd;

private:
    FVector GetRandomPatrolPoint();
    void WakeUp();
    void GoBackToSleep();

    // 공격 빈도를 제어하기 위한 타이머
    UPROPERTY()
    float LastAttackTime;

    UPROPERTY(VisibleAnywhere, Category = "Zombie|Components")
     UPawnSensingComponent* PawnSensingComp;

    UPROPERTY(Replicated)
    FVector PatrolPoint;

    UPROPERTY()
    // 마지막으로 플레이어를 감지한 시간
    float LastSeenTime;

    UPROPERTY()
    class UServerLocationComponent* ServerLocationComp;

    UPROPERTY()
    FTimerHandle SleepTimerHandle;
};