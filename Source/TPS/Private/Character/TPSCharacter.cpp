#include "TPS/Public/Character/TPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Character/TPSAnimInstance.h"
#include "Character/TPSPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Components/WeaponComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"

ATPSCharacter::ATPSCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
	AimWalkSpeed = 350.f;
	BaseWalkSpeed = 800.f;
	TurnInPlace = ETurnInPlace::ETIP_NotTurning;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->JumpZVelocity = 900.f;
	GetCharacterMovement()->GravityScale = 1.5f;
	GetCharacterMovement()->MaxWalkSpeed = 750.f;
	GetCharacterMovement()->RotationRate.Yaw = 720.f;

	GetMesh()->SetRelativeLocation({0, 0, -88.f});
	GetMesh()->SetRelativeRotation({0, -90.f, 0.f});
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->SetRelativeLocation({-70.f, 0, 100.f});
	CameraBoom->TargetArmLength = 250.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	FollowCamera->PostProcessSettings.bOverride_DepthOfFieldFocalDistance = true;
	FollowCamera->PostProcessSettings.DepthOfFieldFocalDistance = 10000.f; // Removes blurriness from far away objects
	FollowCamera->PostProcessSettings.bOverride_DepthOfFieldFstop = true; // Removes blurriness from close objects
	FollowCamera->PostProcessSettings.DepthOfFieldFstop = 32.f;

	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("CombatComponent"));
	WeaponComponent->SetIsReplicated(true);
}

void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (ATPSPlayerController* PlayerController = Cast<ATPSPlayerController>(GetController()))
	{
		WeaponComponent->SetCharacterController(PlayerController);
		
		if (ATPSHUD* HUD = Cast<ATPSHUD>(PlayerController->GetHUD()))
		{
			WeaponComponent->SetCharacterHUD(HUD);
		}
	}
}

void ATPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetAimOffset(DeltaTime);
}

void ATPSCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (WeaponComponent)
	{
		WeaponComponent->SetCharacter(this);
	}

	// }
}

void ATPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ThisClass::EquipWeapon);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ThisClass::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ThisClass::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ThisClass::EndCrouch);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ThisClass::BeginAim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ThisClass::EndAim);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ThisClass::BeginFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ThisClass::EndFire);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPSCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ThisClass::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ThisClass::LookUp);
}

void ATPSCharacter::MoveForward(float Value)

{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation{0.f, Controller->GetControlRotation().Yaw, 0.f};
		const FVector Direction{FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X)};
		AddMovementInput(Direction, Value);
	}
}

void ATPSCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation{0.f, Controller->GetControlRotation().Yaw, 0.f};
		const FVector Direction{FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y)};
		AddMovementInput(Direction, Value);
	}
}

void ATPSCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ATPSCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ATPSCharacter::Jump()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void ATPSCharacter::BeginCrouch()
{
	ACharacter::Crouch();
}

void ATPSCharacter::EndCrouch()
{
	ACharacter::UnCrouch();
}

void ATPSCharacter::BeginAim()
{
	if (!WeaponComponent) return;
	WeaponComponent->StartAiming(true);
}

void ATPSCharacter::EndAim()
{
	if (!WeaponComponent) return;
	WeaponComponent->StartAiming(false);
}

void ATPSCharacter::BeginFire()
{
	if (!WeaponComponent) return;
	WeaponComponent->BeginFire(true);
}

void ATPSCharacter::EndFire()
{
	if (!WeaponComponent) return;
	WeaponComponent->EndFire();
}

void ATPSCharacter::EquipWeapon(AWeapon* Weapon)
{
	if (!WeaponComponent) return;

	if (HasAuthority())
	{
		WeaponComponent->EquipWeapon(Weapon);
	}
	else
	{
		ServerEquipWeapon(Weapon);
	}
}

bool ATPSCharacter::ServerEquipWeapon_Validate(AWeapon* Weapon)
{
	return true;
}

void ATPSCharacter::ServerEquipWeapon_Implementation(AWeapon* Weapon)
{
	if (WeaponComponent)
	{
		WeaponComponent->EquipWeapon(Weapon);
	}
}

void ATPSCharacter::SetAimOffset(float DeltaTime)
{
	if (!IsWeaponEquipped()) return;

	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	const float Speed = Velocity.Size();
	const bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir)
	{
		const FRotator CurrentAimRotation = FRotator{0.f, GetBaseAimRotation().Yaw, 0.f};
		const FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(
			CurrentAimRotation, InitialBaseRotation);
		AimOffsetYaw = DeltaAimRotation.Yaw;

		if (TurnInPlace == ETurnInPlace::ETIP_NotTurning)
		{
			InterpAimOffsetYaw = AimOffsetYaw;
		}
		bUseControllerRotationYaw = true;
		TurningInPlace(DeltaTime);
	}

	if (Speed > 0.f || bIsInAir)
	{
		InitialBaseRotation = FRotator{0.f, GetBaseAimRotation().Yaw, 0.f};
		bUseControllerRotationYaw = true;
		AimOffsetYaw = 0.f;
		TurnInPlace = ETurnInPlace::ETIP_NotTurning;
	}

	AimOffsetPitch = GetBaseAimRotation().Pitch;
	if (AimOffsetPitch > 90.f && !IsLocallyControlled())
	{
		const FVector2D InRange{270.f, 360.f};
		const FVector2D OutRange{-90.f, 0.f};
		AimOffsetPitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AimOffsetPitch);
	}
}

void ATPSCharacter::TurningInPlace(float DeltaTime)
{
	if (AimOffsetYaw > 35.f)
	{
		TurnInPlace = ETurnInPlace::ETIP_Right;
	}
	else if (AimOffsetYaw < -50.f)
	{
		TurnInPlace = ETurnInPlace::ETIP_Left;
	}

	if (TurnInPlace != ETurnInPlace::ETIP_NotTurning)
	{
		InterpAimOffsetYaw = FMath::FInterpTo(InterpAimOffsetYaw, 0.f, DeltaTime, 4.f);
		AimOffsetYaw = InterpAimOffsetYaw;

		if (FMath::Abs(AimOffsetYaw) < 15.f)
		{
			TurnInPlace = ETurnInPlace::ETIP_NotTurning;
			InitialBaseRotation = FRotator{0.f, GetBaseAimRotation().Yaw, 0.f};
		}
	}
}

void ATPSCharacter::PlayFireMontage() const
{
	if (!WeaponComponent || !FireWeaponMontage || !WeaponComponent->IsWeaponEquipped()) return;

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		const FName SectionName = WeaponComponent->IsAiming() ? FName("Aim") : FName("Hip");
		AnimInstance->Montage_JumpToSection(SectionName);
		// WeaponComponent->BeginFire(true);
	}
}

bool ATPSCharacter::IsWeaponEquipped() const
{
	if (!HasAuthority())
	{
		if (WeaponComponent)
		{
			// UE_LOG(LogTemp, Warning, TEXT("Weapon equipped %d"), WeaponComponent->IsWeaponEquipped());
		}
	}
	return WeaponComponent && WeaponComponent->IsWeaponEquipped();
}

bool ATPSCharacter::IsAiming() const
{
	return WeaponComponent && WeaponComponent->IsAiming();
}

AWeapon* ATPSCharacter::GetWeaponEquipped() const
{
	if (!WeaponComponent) return nullptr;
	return WeaponComponent->GetWeaponEquipped();
}

FVector ATPSCharacter::GetHitTarget() const
{
	return WeaponComponent == nullptr ? FVector{} : WeaponComponent->GetHitTarget();
}
