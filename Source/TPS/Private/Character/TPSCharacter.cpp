#include "TPS/Public/Character/TPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/TWeapon.h"

// Sets default values

ATPSCharacter::ATPSCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	GetMesh()->SetRelativeLocation({0, 0, -88.f});
	GetMesh()->SetRelativeRotation({0, -90.f, 0.f});

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->SetRelativeLocation({-100.f, 0, BaseEyeHeight});
	CameraBoom->TargetArmLength = 350.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatComponent->SetIsReplicated(true);
}

// Called when the game starts or when spawned

void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATPSCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (CombatComponent)
	{
		CombatComponent->SetCharacter(this);
	}
}

void ATPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ATPSCharacter, CurrentWeapon, COND_OwnerOnly);
}

void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ThisClass::EquipWeapon);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ThisClass::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ThisClass::EndCrouch);
	
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ThisClass::BeginAim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ThisClass::EndAim);

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
	if(!CombatComponent)
	{
		return;
	}

	CombatComponent->SetAiming(true);
}

void ATPSCharacter::EndAim()
{
	if(!CombatComponent)
	{
		return;
	}

	CombatComponent->SetAiming(false);
}

void ATPSCharacter::EquipWeapon()
{
	if (!CombatComponent)
	{
		return;
	}

	if (HasAuthority())
	{
		CombatComponent->EquipWeapon(CurrentWeapon);
	}
	else
	{
		ServerEquipWeapon();
	}
}

void ATPSCharacter::SetCurrentWeapon(ATWeapon* Weapon)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->ShowPickupWidget(false);
	}

	CurrentWeapon = Weapon;

	if (IsLocallyControlled() && CurrentWeapon != nullptr)
	{
		CurrentWeapon->ShowPickupWidget(true);
	}
}

void ATPSCharacter::OnRep_CurrentWeapon(ATWeapon* LastWeapon)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->ShowPickupWidget(true);
	}

	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

bool ATPSCharacter::ServerEquipWeapon_Validate()
{
	return true;
}

void ATPSCharacter::ServerEquipWeapon_Implementation()
{
	if (CombatComponent)
	{
		CombatComponent->EquipWeapon(CurrentWeapon);
	}
}

bool ATPSCharacter::IsWeaponEquipped() const
{
	return CombatComponent && CombatComponent->GetWeaponEquipped();
}

bool ATPSCharacter::IsAiming() const
{
	return CombatComponent && CombatComponent->IsAiming();
}
