#pragma once

#include "CoreMinimal.h"
#include "TPSAnimInstance.h"
#include "TurnInPlace.h"
#include "GameFramework/Character.h"
#include "TPSCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFireAction, bool, bIsFiring);

class USpringArmComponent;
class UCameraComponent;
class AWeapon;
class UWeaponComponent;
class UAnimMontage;

UCLASS(ClassGroup=TPS)
class TPS_API ATPSCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = Components)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Components)
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, Category = Components)
	UWeaponComponent* WeaponComponent;

	UPROPERTY(EditAnywhere, Category = Weapon)
	UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	float BaseWalkSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	float AimWalkSpeed;

	FRotator InitialBaseRotation;

	ETurnInPlace TurnInPlace;
	
	float AimOffsetYaw;

	float AimOffsetPitch;

	float InterpAimOffsetYaw;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEquipWeapon(AWeapon* Weapon);

protected:
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	virtual void Jump() override;

	virtual void PostInitializeComponents() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void MoveForward(float Value);

	virtual void MoveRight(float Value);

	virtual void Turn(float Value);

	virtual void LookUp(float Value);

	virtual void BeginCrouch();

	virtual void EndCrouch();

	virtual void BeginAim();

	virtual void EndAim();

	virtual void BeginFire();

	virtual void EndFire();

	virtual void SetAimOffset(float DeltaTime);

	virtual void TurningInPlace(float DeltaTime);

public:
	
	ATPSCharacter();

	void PlayFireMontage();
	
	virtual void EquipWeapon(AWeapon* Weapon);

	// void SetWeapon(AWeapon* Weapon) const;

	bool IsWeaponEquipped() const;

	bool IsAiming() const;

	AWeapon* GetWeaponEquipped() const;

	// void SetWeapon(AWeapon* Weapon);

	FORCEINLINE float GetBaseWalkSpeed() const { return BaseWalkSpeed; }
	
	FORCEINLINE float GetAimWalkSpeed() const { return AimWalkSpeed; }

	FORCEINLINE float GetAimOffsetYaw() const { return AimOffsetYaw; }

	FORCEINLINE float GetAimOffsetPitch() const { return AimOffsetPitch; }
	
	FORCEINLINE ETurnInPlace GetTurnInPlace() const { return TurnInPlace; } 
};
