#include "Weapon/Projectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collsion"));
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	SetRootComponent(CollisionBox);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Proejctile Component"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->InitialSpeed = 15000.f;
	ProjectileMovementComponent->InitialSpeed = ProjectileSpeed;
	ProjectileMovementComponent->MaxSpeed = ProjectileSpeed;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	}

	if (GetWorld() && TracerParticleSystem)
	{
		UGameplayStatics::SpawnEmitterAttached(TracerParticleSystem, CollisionBox, FName(), GetActorLocation(),
		                                       GetActorRotation(), EAttachLocation::KeepWorldPosition);
	}
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                        FVector NormalImpulse, const FHitResult& Hit)
{
	Destroyed();
}

void AProjectile::Destroyed()
{
	Super::Destroyed();

	if (const UWorld* World = GetWorld())
	{
		const FVector ProjectileLocation = GetActorLocation();

		if (ImpactParticleSystem)
		{
			UGameplayStatics::SpawnEmitterAtLocation(World, ImpactParticleSystem, ProjectileLocation);
		}

		if (ImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(World, ImpactSound, ProjectileLocation);
		}
	}
}
