#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "KeyItem.generated.h"

UCLASS()
class BLOODYGROUND_API AKeyItem : public AActor
{
    GENERATED_BODY()

public:
    AKeyItem();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UFUNCTION()
        void OnOverlapBegin(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION(NetMulticast, Reliable)
     void Multicast_PlaySoundAndDestroy();

    UPROPERTY(VisibleAnywhere, Category = "Components")
        class UStaticMeshComponent* Mesh;

    UPROPERTY(EditAnywhere, Category = "Effects")
        class USoundCue* PickupSound;

private:
    FVector StartLocation;
    float RunningTime;
};
