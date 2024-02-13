#include "BaseCharacter.h"
#include "Net/UnrealNetwork.h"
#include "BloodyGround/Weapon/BaseWeapon.h"
#include "BloodyGround/Weapon/MachineGun.h"
#include "BloodyGround/Weapon/Pistol.h"
#include "BloodyGround/Component/InventoryComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BloodyGround/BloodyGroundGameModeBase.h"
#include "BloodyGround/HUD/InGameHUD.h"
#include "BloodyGround/HUD/InGameWidget.h"

// 클래스 생성자
ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// 체력 초기화
	Health = 100.0f;

	// 네트워크 복제를 활성화
	SetReplicates(true);
	SetReplicateMovement(true);

	// BattleComponent 추가
	BattleComp = CreateDefaultSubobject<UBattleComponent>(TEXT("BattleComponent"));

	// 카메라 붐 생성 및 설정
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // 카메라와 캐릭터 사이의 거리
	CameraBoom->bUsePawnControlRotation = true; // 카메라가 캐릭터의 회전을 따라감
	CameraBoom->SocketOffset = FVector(0.0f, 50.0f, 50.0f); // 카메라 위치 조정

	// 카메라 생성 및 설정
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // 카메라 붐에 카메라 부착
	FollowCamera->bUsePawnControlRotation = false; // 카메라가 붐의 회전을 따라가지 않게 함

	// FOV 초기값 설정
	DefaultFOV = 90.0f;
	AimedFOV = 65.0f;  // 조준 시 FOV 값
	FOVInterpSpeed = 20.0f; // FOV 변경 속도

	// 인벤토리 컴포넌트 초기화
	InventoryComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	NoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("Noise Emitter"));

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.AddUnique(FName("Player"));

	CharacterState = ECharacterState::None;

	PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController) 
	{
		PlayerHUD = Cast<AInGameHUD>(PlayerController->GetHUD());
	}
	
	// 기본 무기 및 탄알 설정
	if (HasAuthority())
	{
		// 권총 생성 및 설정
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		APistol* NewPistol = GetWorld()->SpawnActor<APistol>(PistolBlueprint, this->GetActorLocation(), this->GetActorRotation(), SpawnParams);
		if (NewPistol)
		{
			InventoryComp->AddWeapon(NewPistol);
			InventoryComp->SetInitWeapon(NewPistol);
			NewPistol->SetActorHiddenInGame(false);

			// 무기를 캐릭터의 소켓에 부착
			const FName WeaponSocketName(TEXT("WeaponSocket_Pistol")); // 소켓 이름
			NewPistol->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);

		}

		// 머신건 생성 및 설정
		AMachineGun* NewMachineGun = GetWorld()->SpawnActor<AMachineGun>(MachineGunBlueprint, this->GetActorLocation(), this->GetActorRotation(), SpawnParams);
		if (NewMachineGun)
		{
			InventoryComp->AddWeapon(NewMachineGun);
			NewMachineGun->SetActorHiddenInGame(true);

			// 무기를 캐릭터의 소켓에 부착
			const FName WeaponSocketName(TEXT("WeaponSocket_Rifle")); // 소켓 이름
			NewMachineGun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);

		}

		// 탄알 설정
		InventoryComp->SetPistolAmmo(50);
		InventoryComp->SetMachineGunAmmo(300);
	}

	// 카메라 기본 FOV 설정
	if (FollowCamera)
	{
		FollowCamera->SetFieldOfView(DefaultFOV);
	}
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 조준 상태에 따른 캐릭터 회전 처리
	HandleAimingRotation();

	// FOV 보간 처리
	InterpFOV(DeltaTime);
}

void ABaseCharacter::HandleAimingRotation()
{
	if (BattleComp && BattleComp->bIsAiming && Controller)
	{
		// 조준 중일 때 컨트롤러의 회전을 따라감
		const FRotator NewRotation = Controller->GetControlRotation();
		SetActorRotation(FRotator(0.0f, NewRotation.Yaw, 0.0f));
	}
	// 조준 중이 아닐 때는 별도의 회전 처리를 하지 않음 (기본 움직임 로직 사용)
}

void ABaseCharacter::HitReactionEnd()
{
	CharacterState = ECharacterState::None;
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 체력 변수 복제 설정
	DOREPLIFETIME(ABaseCharacter, Health);
	DOREPLIFETIME(ABaseCharacter, CharacterState);
}

float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (CharacterState == ECharacterState::Death || DamageCauser && DamageCauser->IsA(ABaseCharacter::StaticClass()))
	{
		return 0.f;
	}

	Health -= DamageAmount;

	if (Health <= 0)
	{
		HandleDeath();
		return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}
	
	CharacterState = ECharacterState::HitReact;

	if (PlayerHUD) {
		PlayerHUD->UpdateHealth(Health / 100.f);
	}
	
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ABaseCharacter::HandleDeath()
{
	CharacterState = ECharacterState::Death;

	// 모든 입력 비활성화
	DisableInput(Cast<APlayerController>(GetController()));

	// 사망 처리 후 5초 뒤에 Respawn 함수 호출
	FTimerHandle RespawnTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ABaseCharacter::Respawn, 5.0f, false);
}

void ABaseCharacter::Respawn()
{
	// GameMode를 통해 새 캐릭터 스폰
	if (ABloodyGroundGameModeBase* GM = Cast<ABloodyGroundGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		GM->RespawnPlayer(Cast<APlayerController>(GetController()));
	}

	// 사망한 캐릭터 제거
	Destroy();
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 이동 및 점프 입력 바인딩
	PlayerInputComponent->BindAxis("MoveForward", this, &ABaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABaseCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ABaseCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ABaseCharacter::LookUp);


	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABaseCharacter::Jump);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ABaseCharacter::Reload);
	PlayerInputComponent->BindAction("ChangeWeapon", IE_Pressed, this, &ABaseCharacter::ChangeWeapon);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ABaseCharacter::AttackButtonPressed);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &ABaseCharacter::AttackButtonReleased);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABaseCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABaseCharacter::AimButtonReleased);
}

void ABaseCharacter::Reload()
{
	InventoryComp->GetCurrentWeapon()->Reload();
}

void ABaseCharacter::ChangeWeapon()
{
	if (HasAuthority())
	{
		InventoryComp->ChangeWeapon();
	}
	else
	{
		ServerChangeWeapon();
	}
}

void ABaseCharacter::ServerChangeWeapon_Implementation()
{
	InventoryComp->ChangeWeapon();
}

bool ABaseCharacter::ServerChangeWeapon_Validate()
{
	return true; // 유효성 검사 로직
}

void ABaseCharacter::ServerFootStep_Implementation()
{
	this->MakeNoise(FootstepLoudness, this, GetActorLocation());
}

// 전/후 이동 처리
void ABaseCharacter::MoveForward(float Value)
{
	if (CharacterState != ECharacterState::None)
	{
		return;
	}

	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// 캐릭터의 방향 찾기
		const FRotator Rotation = Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
		AddMovementInput(Direction, Value);
		ServerFootStep();
	}
}

// 좌/우 이동 처리
void ABaseCharacter::MoveRight(float Value)
{
	if (CharacterState != ECharacterState::None)
	{
		return;
	}

	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// 캐릭터의 방향 찾기
		const FRotator Rotation = Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
		ServerFootStep();
	}
}

void ABaseCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ABaseCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ABaseCharacter::Jump()
{
	if (CharacterState != ECharacterState::None)
	{
		return;
	}

	Super::Jump();
	ServerFootStep();
}

void ABaseCharacter::AttackButtonPressed()
{
	if (InventoryComp && InventoryComp->GetCurrentWeapon())
	{
		InventoryComp->GetCurrentWeapon()->Fire();
	}
}

void ABaseCharacter::AttackButtonReleased()
{
	if (HasAuthority())
	{
		if (InventoryComp && InventoryComp->GetCurrentWeapon())
		{
			InventoryComp->GetCurrentWeapon()->FireEnd();
		}
	}
	else
	{
		ServerStopAttack();
	}
	
}

void ABaseCharacter::ServerStopAttack_Implementation()
{
	MulticastStopAttack();
}

void ABaseCharacter::MulticastStopAttack_Implementation()
{
	if (InventoryComp && InventoryComp->GetCurrentWeapon())
	{
		InventoryComp->GetCurrentWeapon()->FireEnd();
	}
}

void ABaseCharacter::AimButtonPressed()
{
	if (BattleComp)
	{
		BattleComp->StartAiming();  // BattleComponent에 조준 시작을 알림
		StartAiming();              // BaseCharacter 내부에서 추가적인 조준 로직 처리
	}
}

void ABaseCharacter::AimButtonReleased()
{
	if (BattleComp)
	{
		BattleComp->StopAiming();   // BattleComponent에 조준 종료를 알림
		StopAiming();               // BaseCharacter 내부에서 추가적인 조준 해제 로직 처리
	}
}

void ABaseCharacter::StartAiming()
{
}

void ABaseCharacter::StopAiming()
{
}

void ABaseCharacter::InterpFOV(float DeltaTime)
{
	if (FollowCamera)
	{
		float TargetFOV = BattleComp && BattleComp->bIsAiming ? AimedFOV : DefaultFOV;
		float NewFOV = FMath::FInterpTo(FollowCamera->FieldOfView, TargetFOV, DeltaTime, FOVInterpSpeed);
		FollowCamera->SetFieldOfView(NewFOV);
	}
}