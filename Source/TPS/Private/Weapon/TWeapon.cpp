#include "Weapon/TWeapon.h"
#include "Character/TPSCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values

ATWeapon::ATWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	WeaponMesh->SetupAttachment(RootComponent);
	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(WeaponMesh);

	SphereArea = CreateDefaultSubobject<USphereComponent>(TEXT("SphereAreaComponent"));
	SphereArea->SetupAttachment(RootComponent);
	SphereArea->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereArea->SetSphereRadius(65.f);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidgetComponent"));
	PickupWidget->SetupAttachment(RootComponent);
	PickupWidget->SetWidgetSpace(EWidgetSpace::Screen);
	PickupWidget->SetDrawAtDesiredSize(true);
}

// Called when the game starts or when spawned

void ATWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}

	if (HasAuthority())
	{
		SphereArea->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SphereArea->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		SphereArea->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereAreaOverlap);
		SphereArea->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnSphereAreaEndOverlap);
	}
}

void ATWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATWeapon, WeaponState);
}

void ATWeapon::OnSphereAreaOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                   const FHitResult& SweepResult)
{
	if (ATPSCharacter* Player = Cast<ATPSCharacter>(OtherActor))
	{
		Player->SetCurrentWeapon(this);
	}
}

void ATWeapon::OnSphereAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ATPSCharacter* Player = Cast<ATPSCharacter>(OtherActor))
	{
		Player->SetCurrentWeapon(nullptr);
	}
}

void ATWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		break;
	case EWeaponState::EWS_Dropped:
		break;
	default:
		break;
	}
}

void ATWeapon::ShowPickupWidget(bool bShowWidget) const
{
	if (!PickupWidget)
	{
		return;
	}

	PickupWidget->SetVisibility(bShowWidget);
}

void ATWeapon::SetWeaponState(const EWeaponState State)
{
	WeaponState = State;

	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		SphereArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}
