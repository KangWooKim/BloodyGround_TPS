#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BloodyGround/Component/BattleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "BaseCharacter.generated.h"

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	None    UMETA(DisplayName = "None"),
	Attacking  UMETA(DisplayName = "Attacking"),
	HitReact  UMETA(DisplayName = "HitReact"),
	Death  UMETA(DisplayName = "Death"),
	MAX UMETA(DisplayName = "MAX")
};


UCLASS()
class BLOODYGROUND_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	FORCEINLINE float GetHealth() { return Health; }
	
protected:

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void HandleDeath();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerFootStep();

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void StartAiming();
	virtual void StopAiming();

	FORCEINLINE class AInGameHUD* GetInGameHUD() { return PlayerHUD; }

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Health")
	float Health;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBattleComponent* BattleComp;

	FORCEINLINE	ECharacterState GetCharacterState() { return CharacterState; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UInventoryComponent* InventoryComp; // 인벤토리 컴포넌트 추가

	UFUNCTION(Server, Reliable)
	void ServerStopAttack();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStopAttack();

	UFUNCTION()
	void Reload();
	// 무기 변경 함수
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerChangeWeapon();

	UFUNCTION()
	void HandleAimingRotation();

	UFUNCTION(BlueprintCallable)
	void HitReactionEnd();


	

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class APistol> PistolBlueprint;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AMachineGun> MachineGunBlueprint;

	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = Noise)
	class UPawnNoiseEmitterComponent* NoiseEmitter;

	UFUNCTION(BlueprintCallable)
	void Respawn();

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	virtual void Jump() override;

	void AttackButtonPressed();
	void AttackButtonReleased();
	void AimButtonPressed();
	void AimButtonReleased();

	// 조준 관련 변수들
	float DefaultFOV;  // 기본 FOV 값
	float AimedFOV;    // 조준 시 FOV 값
	float FOVInterpSpeed; // FOV 변경 속도

	void InterpFOV(float DeltaTime);

	// 카메라와 카메라 붐 컴포넌트 추가
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UFUNCTION()
	void ChangeWeapon();

	UPROPERTY(Replicated)
	ECharacterState CharacterState;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float FootstepLoudness = 0.2f;

	UPROPERTY()
	class APlayerController* PlayerController;

	UPROPERTY()
	class AInGameHUD* PlayerHUD;
};