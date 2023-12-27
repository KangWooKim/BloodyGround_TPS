#include "BaseZombie.h"
#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "BloodyGround/Character/BaseCharacter.h"
#include "BloodyGround/Weapon/BaseWeapon.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "BloodyGround/Component/ServerLocationComponent.h"

ABaseZombie::ABaseZombie()
{
    PrimaryActorTick.bCanEverTick = true;

    // ��Ʈ��ũ ������ Ȱ��ȭ
    SetReplicates(true);
    SetReplicateMovement(true);


    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
    PawnSensingComp->SightRadius = 5000.f;
    PawnSensingComp->OnSeePawn.AddDynamic(this, &ABaseZombie::OnSeePlayer);
    PawnSensingComp->OnHearNoise.AddDynamic(this, &ABaseZombie::OnHearNoise);

    PawnSensingComp->HearingThreshold = 3000.f;
    PawnSensingComp->LOSHearingThreshold = 2800.f;
    PawnSensingComp->bOnlySensePlayers = false;

    ZombieState = EZombieState::None;
    PatrolRadius = 1000.0f;
    TimeToSleep = 5.0f;

    Health = 50.0f;
    AttackRange = 80.0f;
    Damage = 20.f;

    // ������ ���� �ð� �ʱ�ȭ
    LastAttackTime = -AttackCooldown;

    // ĳ������ ������ ����
    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // ���� ��ġ ������Ʈ �߰�
    ServerLocationComp = CreateDefaultSubobject<UServerLocationComponent>(TEXT("ServerLocationComp"));
    if (ServerLocationComp)
    {
        ServerLocationComp->GetBodyComponent()->SetupAttachment(GetMesh(), FName("Spine"));
        ServerLocationComp->GetLeftLegComponent()->SetupAttachment(GetMesh(), FName("LeftLeg"));
        ServerLocationComp->GetRightLegComponent()->SetupAttachment(GetMesh(), FName("RightLeg"));
    }
   
}

void ABaseZombie::BeginPlay()
{
    Super::BeginPlay();

    ZombieState = EZombieState::None;

    GetCharacterMovement()->MaxWalkSpeed = 150.0f;
    PatrolPoint = GetRandomPatrolPoint();

    Tags.AddUnique(FName("Zombie"));
}

void ABaseZombie::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // �����Ǿ�� �ϴ� ���� �߰�
    DOREPLIFETIME(ABaseZombie, ZombieState);
    DOREPLIFETIME(ABaseZombie, CurrentTarget);
    DOREPLIFETIME(ABaseZombie, PatrolPoint);
    DOREPLIFETIME(ABaseZombie, Health);
}

void ABaseZombie::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    PawnSensingComp->OnSeePawn.AddDynamic(this, &ABaseZombie::OnSeePlayer);
    PawnSensingComp->OnHearNoise.AddDynamic(this, &ABaseZombie::OnHearNoise);
}

void ABaseZombie::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);


    if (ZombieState == EZombieState::Attacking || ZombieState == EZombieState::Sleep || ZombieState == EZombieState::HitReact)
    {
        return;
    }

    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController)
    {
        if (CurrentTarget)
        {
            if (FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation()) <= AttackRange)
            {
                float CurrentTime = GetWorld()->GetTimeSeconds();
                if (CurrentTime - LastAttackTime >= AttackCooldown)
                {
                    Attack(CurrentTarget);
                    LastAttackTime = CurrentTime;
                }
            }
            else if (!IsTargetInSight(CurrentTarget))
            {
                // Ÿ���� �þ� ���� ������ ���� ���
                CurrentTarget = nullptr;
                RestartPatrol();
            }
            else
            {
                // Ÿ���� ���� ������ ������� �þ� ���� �ִ� ���
                ZombieState = EZombieState::None;
                AIController->MoveToActor(CurrentTarget);
            }
        }
        else
        {
            // Ÿ���� ���� ��� ������ ��� ����
            if (AIController->GetPathFollowingComponent()->DidMoveReachGoal())
            {
                PatrolPoint = GetRandomPatrolPoint();
                AIController->MoveToLocation(PatrolPoint);
            }
        }
    }
}


FVector ABaseZombie::GetRandomPatrolPoint()
{
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    FNavLocation ResultLocation;
    if (NavSys && NavSys->GetRandomPointInNavigableRadius(GetActorLocation(), PatrolRadius, ResultLocation))
    {
        return ResultLocation.Location;
    }
    return GetActorLocation();
}

void ABaseZombie::OnSeePlayer(APawn* Pawn)
{
    if (ZombieState == EZombieState::Sleep)
    {
        return;
    }

    if (CurrentTarget) return;

    if (ZombieState == EZombieState::None && Pawn && Pawn->ActorHasTag(FName("Player")))
    {
        AAIController* AIController = Cast<AAIController>(GetController());
        if (AIController)
        {
            AIController->MoveToActor(Pawn);
            CurrentTarget = Pawn;
            LastSeenTime = GetWorld()->GetTimeSeconds();
        }
    }
}

void ABaseZombie::OnHearNoise(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
    if (ZombieState == EZombieState::Sleep)
    {
        return;
    }

    if (NoiseInstigator->ActorHasTag(FName("Zombie")) || CurrentTarget != nullptr)
    {
        return;
    }

    WakeUp();
 
    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController)
    {
        AIController->MoveToActor(NoiseInstigator);
        CurrentTarget = NoiseInstigator;
    }
}

void ABaseZombie::WakeUp()
{
    ZombieState = EZombieState::None;
}

void ABaseZombie::GoBackToSleep()
{
    ZombieState = EZombieState::Sleep;
    // �߰� ����...
    FTimerHandle UnusedHandle;
    GetWorldTimerManager().SetTimer(UnusedHandle, this, &ABaseZombie::GoBackToSleep, TimeToSleep, false);
}

void ABaseZombie::Attack(APawn* Target)
{
    if (ZombieState == EZombieState::None && Target->IsA(ABaseCharacter::StaticClass()))
    {
        ZombieState = EZombieState::Attacking;
    }
}

void ABaseZombie::ApplyDamageToTarget()
{
    if (HasAuthority()) 
    {
        if (CurrentTarget && FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation()) <= AttackRange)
        {
            // ������ ���� ����
            UGameplayStatics::ApplyDamage(CurrentTarget, Damage, GetController(), this, UDamageType::StaticClass());
        }

        ZombieState = EZombieState::None;
    }
    
}

float ABaseZombie::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    UE_LOG(LogTemp, Warning, TEXT("Zombie Takes Damage"));
    if (DamageCauser && DamageCauser->IsA(ABaseZombie::StaticClass()))
    {
        return 0.0f;
    }

    if (ZombieState == EZombieState::Death)
    {
        return 0.0f;
    }

    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    Health -= ActualDamage;

    if (Health <= 0)
    {
        HandleDeath();
    }
    else
    {
        ZombieState = EZombieState::HitReact;
        GetCharacterMovement()->StopMovementImmediately(); // ������ ����
        if (CurrentTarget == nullptr)
        {
            CurrentTarget = Cast<APawn>(DamageCauser);
        }
    }

    return ActualDamage;
}

void ABaseZombie::HitReactEnd()
{
    ZombieState = EZombieState::None;
}



void ABaseZombie::HandleDeath()
{
    ZombieState = EZombieState::Death;
}

void ABaseZombie::DeathEnd()
{
    Super::Destroy();

    Destroy();
}

void ABaseZombie::RestartPatrol()
{
    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController)
    {
        PatrolPoint = GetRandomPatrolPoint();
        AIController->MoveToLocation(PatrolPoint);
    }
}

bool ABaseZombie::IsTargetInSight(APawn* Target)
{
    if (!Target)
    {
        return false;
    }

    // ���� �ð��� ���������� �÷��̾ ������ �ð��� ���� ���
    float TimeSinceLastSeen = GetWorld()->GetTimeSeconds() - LastSeenTime;

    // ���� ���, 5�� �̳��� Ÿ���� �����ߴٸ� �þ� ���� ���� ����
    return TimeSinceLastSeen < 5.0f;
}