#include "KeyItem.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "BloodyGround/Character/BaseCharacter.h"

AKeyItem::AKeyItem()
{
    PrimaryActorTick.bCanEverTick = true;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;

    // 네트워크 복제 활성화
    SetReplicates(true);
    SetReplicateMovement(true);

    // 충돌 구성 요소 추가 및 설정
    USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    SphereComponent->SetSphereRadius(100.0f);
    SphereComponent->SetupAttachment(RootComponent);
    SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AKeyItem::OnOverlapBegin);
}

void AKeyItem::BeginPlay()
{
    Super::BeginPlay();
    StartLocation = GetActorLocation();
}

void AKeyItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 위아래 움직임 구현
    FVector NewLocation = StartLocation;
    RunningTime += DeltaTime;
    float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
    NewLocation.Z += DeltaHeight * 20.0f; // 조정 가능한 매개변수
    SetActorLocation(NewLocation);
}

void AKeyItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (HasAuthority()) // 서버에서만 실행
    {
        ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(OtherActor);
        if (BaseCharacter && OtherActor != this && OtherComp)
        {
            // 서버에서 효과음 재생 및 아이템 제거 처리
            Multicast_PlaySoundAndDestroy();
        }
    }
}

void AKeyItem::Multicast_PlaySoundAndDestroy_Implementation()
{
    // 모든 클라이언트에 효과음 재생
    if (PickupSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
    }

    // 아이템 제거
    Destroy();
}