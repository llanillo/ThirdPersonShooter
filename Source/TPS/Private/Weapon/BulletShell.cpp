#include "Weapon/BulletShell.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

ABulletShell::ABulletShell()
{
	ShellMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	ShellMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	ShellMeshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	ShellMeshComponent->SetSimulatePhysics(true);
	ShellMeshComponent->SetEnableGravity(true);
	ShellMeshComponent->SetNotifyRigidBodyCollision(true);
	SetRootComponent(ShellMeshComponent);
	
}

void ABulletShell::BeginPlay()
{
	Super::BeginPlay();

	AActor::SetLifeSpan(BulletLifeSpan);
	
	ShellMeshComponent->AddImpulse(GetActorForwardVector() * EjectionImpulse);

	if (HasAuthority())
	{
		ShellMeshComponent->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	}
}

void ABulletShell::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                         FVector NormalImpulse, const FHitResult& Hit)
{
	if(bAlreadyPlayedSound) return;
	
	if (const UWorld* World = GetWorld())
	{
		if (DropSound)
		{
			UGameplayStatics::PlaySoundAtLocation(World, DropSound, GetActorLocation());
			bAlreadyPlayedSound = true;
		}
	}
}