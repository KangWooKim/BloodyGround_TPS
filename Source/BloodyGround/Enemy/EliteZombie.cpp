// Fill out your copyright notice in the Description page of Project Settings.


#include "EliteZombie.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/DamageType.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"

AEliteZombie::AEliteZombie()
{
	PrimaryActorTick.bCanEverTick = true;

	// 네트워크 복제를 활성화
	SetReplicates(true);
	SetReplicateMovement(true);

	PatrolRadius = 1000.0f;
	TimeToSleep = 10.0f;

	Health = 1000000.0f;
	AttackRange = 80.0f;
	Damage = 50.f;
	AttackCooldown = 3.f;

	LegDamageAccumulated = 0.f;
}

void AEliteZombie::BeginPlay()
{
	Super::BeginPlay();
}

float AEliteZombie::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// HitLocation 정보를 가져옴
	FVector HitLocation;
	if (ZombieInjuryState == EZombieInjuryState::Injured && DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		FPointDamageEvent* PointDmg = (FPointDamageEvent*)&DamageEvent;
		HitLocation = PointDmg->HitInfo.ImpactPoint;

		// 다리에 명중했는지 확인 (가정: 다리의 본 이름이 "LegBone"이라고 가정)
		if (PointDmg->HitInfo.BoneName == FName("LegBone"))
		{
			// 다리에 명중 시 피해량 조정
			LegDamageAccumulated += DamageAmount;
		}

		if (ZombieInjuryState == EZombieInjuryState::None && LegDamageAccumulated >= 100)
		{
			GetDown();
		}
	}

	// 기본 피해 처리 로직
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}


bool AEliteZombie::IsShotInLeg(const FHitResult& HitResult)
{
	// 'Leg'이라는 태그가 부착된 메시 부분을 히트했는지 확인
	return HitResult.BoneName.ToString().Contains("Leg");
}

void AEliteZombie::GetDown()
{
	ZombieInjuryState = EZombieInjuryState::Injured;
}

void AEliteZombie::StandUpEnd()
{
	ZombieInjuryState = EZombieInjuryState::None;

	LegDamageAccumulated = 0.f;
}

void AEliteZombie::Attack(APawn* Target)
{
	Super::Attack(Target);
	
}

void AEliteZombie::ApplyDamageToTarget()
{
	Super::ApplyDamageToTarget();
}

void AEliteZombie::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEliteZombie, ZombieInjuryState);
	DOREPLIFETIME(AEliteZombie, LegDamageAccumulated);
}
