// Copyright Epic Games, Inc. All Rights Reserved.


#include "BloodyGroundGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "BloodyGround/Character/BaseCharacter.h"

ABloodyGroundGameModeBase::ABloodyGroundGameModeBase()
{
    // ABaseCharacter 클래스를 DefaultPawnClass로 설정
    DefaultPawnClass = ABaseCharacter::StaticClass();
}

void ABloodyGroundGameModeBase::RespawnPlayer(APlayerController* PC)
{
    if (!PC || !HasAuthority()) // 서버 권한 확인
    {
        return;
    }

    // PlayerStart 액터를 모두 찾음
    TArray<AActor*> PlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

    // PlayerStart 액터가 없으면 리턴
    if (PlayerStarts.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No PlayerStarts found on map."));
        return;
    }

    // 랜덤한 PlayerStart 선택
    int32 Index = FMath::RandRange(0, PlayerStarts.Num() - 1);
    APlayerStart* ChosenStart = Cast<APlayerStart>(PlayerStarts[Index]);

    if (!ChosenStart)
    {
        UE_LOG(LogTemp, Warning, TEXT("Selected PlayerStart is not valid."));
        return;
    }

    // 새 캐릭터 스폰 위치 및 회전 설정
    FVector SpawnLocation = ChosenStart->GetActorLocation();
    FRotator SpawnRotation = ChosenStart->GetActorRotation();

    // 캐릭터 클래스가 유효한지 확인
    if (!DefaultPawnClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("DefaultPawnClass is not set."));
        return;
    }

    // 새 캐릭터 스폰
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    ABaseCharacter* NewCharacter = GetWorld()->SpawnActor<ABaseCharacter>(DefaultPawnClass, SpawnLocation, SpawnRotation, SpawnParams);

    if (NewCharacter)
    {
        // 새 캐릭터를 플레이어 컨트롤러에 할당
        PC->Possess(NewCharacter);
    }
}
