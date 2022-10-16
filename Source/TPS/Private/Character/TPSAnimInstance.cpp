#include "Character/TPSAnimInstance.h"
#include "Character/TPSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon/Weapon.h"

void UTPSAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<ATPSCharacter>(TryGetPawnOwner());
	RotationInterpolationSpeed = 6.f;
}

void UTPSAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!Character) return;

	SetMovementAnimation(DeltaSeconds);
	SetWeaponAnimation(DeltaSeconds);
	SetLeftHandInverseKinematics();
}

void UTPSAnimInstance::SetLeftHandInverseKinematics()
{
	if (!Character->GetWeaponEquipped()) return;
	EquippedWeapon = Character->GetWeaponEquipped();

	if (bHasWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && Character->GetMesh())
	{
		FVector OutBoneLocation;
		FRotator OutBoneRotation;
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(LeftHand, RTS_World);
		Character->GetMesh()->TransformToBoneSpace(RightHand, LeftHandTransform.GetLocation(),
		                                           FRotator::ZeroRotator, OutBoneLocation, OutBoneRotation);
		LeftHandTransform.SetLocation(OutBoneLocation);
		LeftHandTransform.SetRotation(FQuat{OutBoneRotation});
	}
}

void UTPSAnimInstance::SetMovementAnimation(float DeltaSeconds)
{
	/* Character movement */
	FVector Velocity = Character->GetVelocity();
	Velocity.Z = 0.f;

	Speed = Velocity.Size();
	bIsInAir = Character->GetMovementComponent()->IsFalling();
	bIsCrouched = Character->bIsCrouched;
	bIsAccelerating = Character->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;
	TurnInPlace = Character->GetTurnInPlace();

	/* Character Rotation */
	LastCharacterRotation = CharacterRotation;
	CharacterRotation = Character->GetActorRotation();

	/* Character Yaw Rotation */
	const FRotator AimRotation = Character->GetBaseAimRotation();
	const FRotator MovementRotation = FRotationMatrix::MakeFromX(Character->GetVelocity()).Rotator();
	const FRotator DeltaRotationYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	LastRotationDelta = FMath::RInterpTo(LastRotationDelta, DeltaRotationYaw, DeltaSeconds, RotationInterpolationSpeed);
	YawOffset = LastRotationDelta.Yaw;

	/* Character Lean Rotation */
	const FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, LastCharacterRotation);
	const float Target = DeltaRotation.Yaw / DeltaSeconds;
	const float Interpolation = FMath::FInterpTo(Lean, Target, DeltaSeconds, RotationInterpolationSpeed);
	Lean = FMath::Clamp(Interpolation, -90.f, 90.f);
}

void UTPSAnimInstance::SetWeaponAnimation(float DeltaSeconds)
{
	/* Character equipped state */
	bHasWeaponEquipped = Character->IsWeaponEquipped();
	bIsAiming = Character->IsAiming();

	/* Aim offsets */
	AimOffsetYaw = Character->GetAimOffsetYaw();
	AimOffsetPitch = Character->GetAimOffsetPitch();
}