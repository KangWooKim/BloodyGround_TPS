#include "BaseWeapon.h"
#include "Animation/AnimationAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "BloodyGround/Character/BaseCharacter.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "BloodyGround/Component/InventoryComponent.h"
#include "BloodyGround/PlayerController/BloodyGroundPlayerController.h"
#include "BloodyGround/Enemy/EliteZombie.h"
#include "BloodyGround/Component/ServerLocationComponent.h"


ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

    // 복제 가능 여부 설정. 이 액터는 복제 가능합니다.
    bReplicates = true;

    // 움직임 복제 여부 설정. 이 액터의 움직임은 복제됩니다.
    SetReplicateMovement(true);
    SetReplicates(true);

    // 스켈레탈 메시 컴포넌트 초기화 
    
    SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
    SetRootComponent(SkeletalMeshComponent);

    // 무기 메시의 충돌 응답 설정
    SkeletalMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
    SkeletalMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
    SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    NoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("Noise Emitter"));

    HitThreshold = 100.f;
}

void ABaseWeapon::ChangeWeapon()
{
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

    WeaponState = EWeaponState::None;

    Character = Cast<ABaseCharacter>(GetOwner());
}

void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABaseWeapon, WeaponState);
    DOREPLIFETIME(ABaseWeapon, CurrentAmmo);
}

void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseWeapon::Fire()
{
    WeaponState = EWeaponState::Fire;

    if (Character)
    {
        FVector StartLocation = SkeletalMeshComponent->GetSocketLocation(TEXT("MuzzleFlash"));
        FVector ForwardVector = Character->GetControlRotation().Vector();
        FVector EndLocation = StartLocation + ForwardVector * 10000.0f;

        FHitResult HitResult;
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(this);
        QueryParams.AddIgnoredActor(Character);

        if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams))
        {
            ABaseZombie* HitZombie = Cast<ABaseZombie>(HitResult.GetActor());
            if (HitZombie)
            {
                if (Character->HasAuthority())
                {
                    FLocationTimeData TempLocationData = HitZombie->GetServerLocationComponent()->GetLocationData();
                    FHitResultData HitResultData = HitZombie->GetServerLocationComponent()->ServerTrace(StartLocation, EndLocation, TempLocationData);
                    if (HitResultData.bHitBody)
                    {
                        HitZombie->TakeShot(HitResultData, Damage);
                        UGameplayStatics::ApplyDamage(HitZombie, Damage, Character->GetController(), this, UDamageType::StaticClass());
                    }
                }
                else if (Character->IsLocallyControlled())
                {
                    // 클라이언트에서 적중 여부를 서버로 전송
                    ServerCheckHit(HitResult, GetWorld()->GetTimeSeconds(), StartLocation, ForwardVector);
                }
            }
        }
    }

    // 발사 애니메이션 재생
    ClientPlayFireAnimation();

    // 소음 발생
    MakeNoise(1.0f); // 소음의 크기를 조절할 수 있습니다.
}

    void ABaseWeapon::ServerCheckHit_Implementation(FHitResult ClientHitResult, float HitTime, FVector StartLocation, FVector EndDirection)
    {
        ABaseZombie* HitZombie = Cast<ABaseZombie>(ClientHitResult.GetActor());
        if (!HitZombie) return;

        ABloodyGroundPlayerController* PlayerController = Cast<ABloodyGroundPlayerController>(Character->GetController());
        if (!PlayerController) return;

        float RTT = PlayerController->GetRoundTripTime();
        UServerLocationComponent* ServerLocationComp = HitZombie->FindComponentByClass<UServerLocationComponent>();
        if (!ServerLocationComp) return;

        float CorrectedTime = HitTime - (RTT / 2.0f);
        FLocationTimeData LocationData = ServerLocationComp->GetInterpolatedLocationData(CorrectedTime);
        FVector EndLocation = StartLocation + EndDirection * 10000.0f;

        FHitResultData HitResults = ServerLocationComp->CheckHitWithTrace(StartLocation, EndLocation, LocationData);

        // 몸통에 적중한 경우
        if (HitResults.bHitBody)
        {
            HitZombie->TakeShot(HitResults, Damage);
            UGameplayStatics::ApplyDamage(HitZombie, Damage, Character->GetController(), this, UDamageType::StaticClass());
        }
    }

bool ABaseWeapon::ServerCheckHit_Validate(FHitResult HitResult, float HitTime, FVector StartLocation, FVector EndDirection)
{
    return true; // 추가적인 유효성 검사 로직이 필요할 수 있습니다
}

void ABaseWeapon::MakeNoise(float Loudness)
{
    if (Character)
    {
        Character->MakeNoise(Loudness, Character, GetActorLocation());
    }
}

bool ABaseWeapon::CanFire()
{
    return true;
}

void ABaseWeapon::FireEnd()
{
    WeaponState = EWeaponState::None;
}

void ABaseWeapon::ReloadEnd()
{
    WeaponState = EWeaponState::None;
}

void ABaseWeapon::Reload()
{
    if (Character == nullptr || Character->InventoryComp == nullptr)
    {
        return; // 캐릭터 또는 인벤토리 컴포넌트가 없는 경우 리턴
    }

    if (WeaponState != EWeaponState::None) return;

    WeaponState = EWeaponState::Reload;

    int32 AmmoToReload = FMath::Min(Capacity - CurrentAmmo, Character->InventoryComp->GetPistolAmmo());
    CurrentAmmo += AmmoToReload;
    Character->InventoryComp->SetPistolAmmo(Character->InventoryComp->GetPistolAmmo() - AmmoToReload);
   
}

EWeaponType ABaseWeapon::GetCurrentWeaponType()
{
	return EWeaponType::None;
}

void ABaseWeapon::ClientPlayFireAnimation()
{
    if (WeaponState == EWeaponState::Fire && FireAnimation)
    {
        // SkeletalMeshComponent가 FireAnimation 애니메이션 몽타주를 재생합니다.
        SkeletalMeshComponent->PlayAnimation(FireAnimation, false);
    }
}