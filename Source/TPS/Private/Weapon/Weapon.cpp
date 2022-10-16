#include "Weapon/Weapon.h"
#include "Weapon/BulletShell.h"
#include "Character/TPSCharacter.h"
#include "Components/PickupComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	WeaponMesh->SetupAttachment(RootComponent);
	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(WeaponMesh);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetWidgetSpace(EWidgetSpace::Screen);
	PickupWidget->SetDrawAtDesiredSize(true);
	PickupWidget->SetRelativeLocation({0, 20.f, 30.f});
	PickupWidget->SetupAttachment(WeaponMesh);

	PickupComponent = CreateDefaultSubobject<UPickupComponent>(TEXT("PickupComponent"));
	PickupComponent->SetRelativeLocation({0, 20.f,0});
	PickupComponent->SetupAttachment(WeaponMesh);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}

	if (HasAuthority())
	{
		PickupComponent->OnPickup.AddDynamic(this, &ThisClass::OnPickup);
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
}

void AWeapon::Fire(const FVector& HitTarget)
{
	/* Plays weapon animation */
	if (!FireAnimation) return;
	WeaponMesh->PlayAnimation(FireAnimation, false);

	/* Spawns bullet shells */
	if(!BulletShellClass) return;
	if(UWorld* const World = GetWorld())
	{
		if(const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(AmmoSocketName))
		{
			const FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);
			ABulletShell* BulletShell = World->SpawnActor<ABulletShell>(BulletShellClass, SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator());
		}
	}
}

void AWeapon::OnPickup(ATPSCharacter* Character)
{
	if (!Character) return;
	Character->EquipWeapon(this);
}

void AWeapon::ShowPickupWidget(bool bShowWidget) const
{
	if (!PickupWidget)
	{
		return;
	}

	PickupWidget->SetVisibility(bShowWidget);
}

void AWeapon::SetWeaponState(const EWeaponState State)
{
	WeaponState = State;

	switch (WeaponState)
	{
	case EWeaponState::Ews_Equipped:
		ShowPickupWidget(false);
		// SphereArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::Ews_Initial: break;
	case EWeaponState::Ews_Dropped: break;
	case EWeaponState::Ews_Max: break;
	default: ;
	}
}

void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::Ews_Equipped:
		ShowPickupWidget(false);
		break;
	case EWeaponState::Ews_Dropped:
		break;
	default:
		break;
	}
}
