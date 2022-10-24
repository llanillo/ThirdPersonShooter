#include "Components/WeaponComponent.h"
#include "Weapon/Weapon.h"
#include "Character/TPSCharacter.h"
#include "Character/TPSPlayerController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraComponent.h"

UWeaponComponent::UWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	CharacterHandSocket = "GripPoint";
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Character)
	{
		DefaultFOV = Character->GetCameraComponent()->FieldOfView;
		CurrentFOV = DefaultFOV;
	}
}

void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceLineFromCrosshair(HitResult);
		HitTarget = HitResult.ImpactPoint;

		InterpFOV(DeltaTime);
		SetCrossHairsHUD(DeltaTime);
	}
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponComponent, WeaponEquipped);
	DOREPLIFETIME(UWeaponComponent, bIsAiming);
}

void UWeaponComponent::SetCrossHairsHUD(const float DeltaTime)
{
	if (!Character || !PlayerController || !PlayerHUD) return;

	FCrossHairHUD CrossHairHUD;
	if (WeaponEquipped)
	{
		CrossHairHUD.CrossHairCenter = WeaponEquipped->GetCrossHairHUD().CrossHairCenter;
		CrossHairHUD.CrossHairBottom = WeaponEquipped->GetCrossHairHUD().CrossHairBottom;
		CrossHairHUD.CrossHairLeft = WeaponEquipped->GetCrossHairHUD().CrossHairLeft;
		CrossHairHUD.CrossHairRight = WeaponEquipped->GetCrossHairHUD().CrossHairRight;
		CrossHairHUD.CrossHairTop = WeaponEquipped->GetCrossHairHUD().CrossHairTop;
	}
	else
	{
		CrossHairHUD = {nullptr, nullptr, nullptr, nullptr, nullptr};
	}

	const FVector2D WalkSpeedRange{0.f, Character->GetMovementComponent()->GetMaxSpeed()};
	const FVector2D VelocityMultiplierRange{0.f, 1.f};
	FVector Velocity = Character->GetVelocity();
	Velocity.Z = 0.f;

	const float CrossHairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange,
	                                                                        Velocity.Size());

	if (Character->GetCharacterMovement()->IsFalling())
	{
		CrossHairAirFactor = FMath::FInterpTo(CrossHairAirFactor, 2.f, DeltaTime, 2.f);
	}
	else
	{
		CrossHairAirFactor = FMath::FInterpTo(CrossHairAirFactor, 0.f, DeltaTime, 5.f);
	}

	CrossHairHUD.Spread = CrossHairAirFactor + CrossHairVelocityFactor;
	PlayerHUD->SetCrossHairHUD(CrossHairHUD);
}

void UWeaponComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceTargetHit)
{
	MulticastFire(TraceTargetHit);
}

void UWeaponComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceTargetHit)
{
	if (!Character || !IsWeaponEquipped()) return;
	Character->PlayFireMontage();
	WeaponEquipped->Fire(TraceTargetHit);
}

void UWeaponComponent::BeginFire(bool bIsShooting)
{
	bIsFiring = bIsShooting;
	FHitResult HitResult;
	TraceLineFromCrosshair(HitResult);
	ServerFire(HitResult.ImpactPoint);
}

void UWeaponComponent::EndFire()
{
	bIsFiring = false;
}

void UWeaponComponent::InterpFOV(float DeltaTime)
{
	if (!WeaponEquipped) return;

	if (bIsAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, WeaponEquipped->GetZoomFOV(), DeltaTime,
		                              WeaponEquipped->GetZoomInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, WeaponEquipped->GetZoomInterpSpeed());
	}

	if (Character && Character->GetCameraComponent())
	{
		Character->GetCameraComponent()->SetFieldOfView(CurrentFOV);
	}
}

void UWeaponComponent::TraceLineFromCrosshair(FHitResult& OutHitResult)
{
	const UWorld* World = GetWorld();
	if (!GEngine || !GEngine->GameViewport || !World) return;

	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	const FVector2D CrossHairLocation = FVector2D{ViewportSize.X / 2.f, ViewportSize.Y / 2.f};
	FVector CrossHairWorldPosition;
	FVector CrossHairWorldDirection;

	if (!UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(World, 0), CrossHairLocation,
	                                              CrossHairWorldPosition, CrossHairWorldDirection))
		return;

	const FVector Start = CrossHairWorldPosition;
	const FVector End = Start + CrossHairWorldDirection * TRACE_LENGTH;

	if (!GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECC_Visibility))
	{
		OutHitResult.ImpactPoint = End;
	}

	UE_LOG(LogTemp, Warning, TEXT("Vector : %s"), *OutHitResult.ToString());
	DrawDebugSphere(World, OutHitResult.ImpactPoint, 32.f, 12, FColor::Yellow);
}

void UWeaponComponent::OnRep_WeaponEquipped(AWeapon* LastWeapon)
{
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

void UWeaponComponent::EquipWeapon(AWeapon* Weapon)
{
	if (!Character || !Weapon) return;

	WeaponEquipped = Weapon;
	WeaponEquipped->SetWeaponState(EWeaponState::Ews_Equipped);
	WeaponEquipped->SetOwner(Character);

	if (const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(CharacterHandSocket))
	{
		HandSocket->AttachActor(WeaponEquipped, Character->GetMesh());
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}

	// WeaponEquipped->AttachToActor(Character, FAttachmentTransformRules::SnapToTargetNotIncludingScale, CharacterHandSocket);
}

void UWeaponComponent::StartAiming(bool bAiming)
{
	if (!Character) return;
	bIsAiming = bAiming;
	ServerStartAiming(bAiming);
	Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming
		                                                  ? Character->GetAimWalkSpeed()
		                                                  : Character->GetBaseWalkSpeed();
}

void UWeaponComponent::ServerStartAiming_Implementation(bool bAiming)
{
	if (!Character) return;
	bIsAiming = bAiming;
	Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming
		                                                  ? Character->GetAimWalkSpeed()
		                                                  : Character->GetBaseWalkSpeed();
}
