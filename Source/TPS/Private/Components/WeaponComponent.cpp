#include "Components/WeaponComponent.h"
#include "Weapon/Weapon.h"
#include "Character/TPSCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

UWeaponComponent::UWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	CharacterHandSocket = "GripPoint";
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponComponent, WeaponEquipped);
	DOREPLIFETIME(UWeaponComponent, bIsAiming);
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

void UWeaponComponent::TraceLineFromCrosshair(FHitResult& OutHitResult)
{
	const UWorld* World = GetWorld();
	if (!GEngine || !GEngine->GameViewport || !World) return;

	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	FVector2D CrosshairLocation = FVector2D{ViewportSize.X / 2.f, ViewportSize.Y / 2.f};
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	if (!UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(World, 0), CrosshairLocation,
	                                              CrosshairWorldDirection, CrosshairWorldDirection))
		return;

	const FVector Start = CrosshairWorldPosition;
	const FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

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
