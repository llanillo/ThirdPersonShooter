#include "Weapon/ProjectileWeapon.h"
#include "Weapon/Projectile.h"
#include "Engine/SkeletalMeshSocket.h"

AProjectileWeapon::AProjectileWeapon() : AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AProjectileWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectileWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	if(!HasAuthority()) return;
	if (!ProjectileClass) return;

	if (const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(GetMuzzleSocketName()))
	{
		if (APawn* CurrentInstigator = Cast<APawn>(GetOwner()))
		{
			if (UWorld* World = GetWorld())
			{
				const FTransform MuzzleTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
				const FVector ToTarget = HitTarget - MuzzleTransform.GetLocation();
				const FRotator MuzzleRotation = ToTarget.Rotation();
				
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.Instigator = CurrentInstigator;
				SpawnParameters.Owner = GetOwner();
				World->SpawnActor<AProjectile>(ProjectileClass, MuzzleTransform.GetLocation(), MuzzleRotation, SpawnParameters);
			}
		}
	}
}
