#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/CapsuleComponent.h"
#include "ServerLocationComponent.generated.h"

class UCapsuleComponent;

// 위치와 시간을 함께 저장하는 구조체
USTRUCT(BlueprintType)
struct FComponentLocationData {
    GENERATED_BODY()

        FVector Location;
    FRotator Rotation;

    FComponentLocationData() : Location(FVector::ZeroVector), Rotation(FRotator::ZeroRotator) {}
    FComponentLocationData(FVector InLocation, FRotator InRotation) : Location(InLocation), Rotation(InRotation) {}
};

USTRUCT(BlueprintType)
struct FLocationTimeData {
    GENERATED_BODY()

    FComponentLocationData CapsuleData;
    FComponentLocationData LeftLegData;
    FComponentLocationData RightLegData;
    float TimeStamp;

    FLocationTimeData() : TimeStamp(0.f) {}
    FLocationTimeData(FComponentLocationData InCapsule, FComponentLocationData InLeftLeg, FComponentLocationData InRightLeg, float InTimeStamp) : CapsuleData(InCapsule), LeftLegData(InLeftLeg), RightLegData(InRightLeg), TimeStamp(InTimeStamp) {}
};

USTRUCT(BlueprintType)
struct FHitResultData {
    GENERATED_BODY()

        bool bHitBody;
    bool bHitLeg;

    FHitResultData() : bHitBody(false), bHitLeg(false) {}
    FHitResultData(bool InHitBody, bool InHitLeg) : bHitBody(InHitBody), bHitLeg(InHitLeg) {}
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BLOODYGROUND_API UServerLocationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UServerLocationComponent();

    FORCEINLINE UCapsuleComponent* GetBodyComponent() { return CapsuleComponent; }
    FORCEINLINE UCapsuleComponent* GetLeftLegComponent() { return LeftLegComponent; }
    FORCEINLINE UCapsuleComponent* GetRightLegComponent() { return RightLegComponent; }

    UFUNCTION(BlueprintCallable, Category = "ServerLocation")
    FLocationTimeData GetInterpolatedLocationData(float Time);

    UFUNCTION(BlueprintCallable, Category = "ServerLocation")
        FHitResultData CheckHitWithTrace(const FVector& StartTrace, const FVector& EndTrace, const FLocationTimeData& LocationData);

    UFUNCTION(BlueprintCallable, Category = "ServerLocation")
        FHitResultData ServerTrace(const FVector& StartTrace, const FVector& EndTrace, const FLocationTimeData& LocationData);

    UFUNCTION(BlueprintCallable, Category = "ServerLocation")
    FLocationTimeData GetLocationData();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    
    UPROPERTY()
    TArray<FLocationTimeData> LocationHistory;

    UPROPERTY()
    float TimeToKeepData; // 오래된 데이터를 얼마나 오래 보관할지
    UPROPERTY()
    float RecordInterval; // 데이터를 얼마나 자주 기록할지
    UPROPERTY()
    float LastRecordTime; // 마지막으로 데이터를 기록한 시간

    UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UCapsuleComponent* CapsuleComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UCapsuleComponent* LeftLegComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UCapsuleComponent* RightLegComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UCapsuleComponent* BodyHitCapsule;

    UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UCapsuleComponent* LeftLegHitCapsule;

    UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
     UCapsuleComponent* RightLegHitCapsule;

    TArray<UCapsuleComponent*> HitCapsules;

    UFUNCTION()
    void RecordLocation();

    UFUNCTION()
    void CleanupOldData();

    void UpdateHitCapsule(UCapsuleComponent* HitCapsule, const FComponentLocationData& LocationData);
    bool PerformLineTrace(const FVector& StartTrace, const FVector& EndTrace, UCapsuleComponent* PrimaryCapsule);
    void RestoreCollisionSettings();

};