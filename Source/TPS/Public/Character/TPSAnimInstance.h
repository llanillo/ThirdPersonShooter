#pragma once

#include "CoreMinimal.h"
#include "TurnInPlace.h"
#include "Animation/AnimInstance.h"
#include "TPSAnimInstance.generated.h"

class ATPSCharacter;
class AWeapon;

UCLASS(ClassGroup=TPS)
class TPS_API UTPSAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = true))
	ATPSCharacter* Character;

	UPROPERTY(BlueprintReadOnly, Category = IK, meta = (AllowPrivateAccess = true))
	FTransform LeftHandTransform;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	ETurnInPlace TurnInPlace;

	UPROPERTY(BlueprintReadOnly, Category = IK, meta = (AllowPrivateAccess = true))
	FName RightHand = "RightHand";
	
	UPROPERTY(BlueprintReadOnly, Category = IK, meta = (AllowPrivateAccess = true))
	FName LeftHand = "LeftHand";
	
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	bool bIsInAir;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	bool bIsAccelerating;

	UPROPERTY(BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
	bool bHasWeaponEquipped;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	bool bIsCrouched;

	UPROPERTY(BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
	bool bIsAiming;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	float YawOffset;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	float Lean;

	UPROPERTY(BlueprintReadOnly, Category = Aim, meta = (AllowPrivateAccess = true))
	float AimOffsetYaw;

	UPROPERTY(BlueprintReadOnly, Category = Aim, meta = (AllowPrivateAccess = true))
	float AimOffsetPitch;
	
public:

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:

	AWeapon* EquippedWeapon;
	
	FRotator LastCharacterRotation;

	FRotator CharacterRotation;

	FRotator LastRotationDelta;

	
	float RotationInterpolationSpeed;
	
	void SetMovementAnimation(float DeltaSeconds);

	// void SetAnimationYawOffset(const float DeltaSeconds);
	
	void SetWeaponAnimation(float DeltaSeconds);
	
	// void SetAnimationAim();

	void SetLeftHandInverseKinematics();
};
