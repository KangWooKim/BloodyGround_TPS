// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseZombie.h"
#include "EliteZombie.generated.h"


UENUM(BlueprintType)
enum class EZombieInjuryState : uint8
{
	None    UMETA(DisplayName = "None"),
	Injured  UMETA(DisplayName = "Injured"),
	Down  UMETA(DisplayName = "Down"),
	Stand  UMETA(DisplayName = "Stand")
};


/**
 * 
 */
UCLASS()
class BLOODYGROUND_API AEliteZombie : public ABaseZombie
{
	GENERATED_BODY()

public:

	AEliteZombie();
	FORCEINLINE EZombieInjuryState GetZombieInjuryState() { return ZombieInjuryState; }

	// 오버라이드된 데미지 처리 함수
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, Category = "Zombie|Combat")
	void GetDown();

	UFUNCTION(BlueprintCallable, Category = "Zombie|Combat")
	void DownEnd();

	UFUNCTION(BlueprintCallable, Category = "Zombie|Combat")
	void StandUp();

	UFUNCTION(BlueprintCallable, Category = "Zombie|Combat")
	void StandUpEnd();

	virtual void Attack(APawn* Target) override;

	virtual void ApplyDamageToTarget() override;

	virtual void TakeShot(FHitResultData HitResult, float WeaponDamage) override;

protected:

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	// 다리에 대한 데미지 누적
	UPROPERTY(Replicated)
	float LegDamageAccumulated;

private:

	UPROPERTY(Replicated)
	EZombieInjuryState ZombieInjuryState;
	
};
