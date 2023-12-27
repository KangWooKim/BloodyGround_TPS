#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BattleComponent.generated.h"



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BLOODYGROUND_API UBattleComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBattleComponent();

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combat")
        bool bIsAiming;

    
    UFUNCTION(BlueprintCallable)
        void StartAiming();

    UFUNCTION(BlueprintCallable)
        void StopAiming();

    UFUNCTION()
        void StartFire();

private:


};
