#include "Weapon/BulletShell.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ABulletShell::ABulletShell()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShellMesh"));
	MeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	MeshComponent->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetEnableGravity(true);
	MeshComponent->SetNotifyRigidBodyCollision(true);
	SetRootComponent(MeshComponent);
}

void ABulletShell::BeginPlay()
{
	Super::BeginPlay();
	MeshComponent->AddImpulse(GetActorForwardVector() * EjectionImpulse);
}

void ABulletShell::OnDestroyTimer()
{
	if (DestroyHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(DestroyHandle);
	}

	Destroyed();
}

void ABulletShell::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                         FVector NormalImpulse, const FHitResult& Hit)
{
	GetWorldTimerManager().SetTimer(DestroyHandle, this, &ThisClass::OnDestroyTimer, GetLifeSpan(), false);
}

void ABulletShell::Destroyed()
{
	Super::Destroyed();

	if (const UWorld* World = GetWorld())
	{
		if (DropSound)
		{
			UGameplayStatics::PlaySoundAtLocation(World, DropSound, GetActorLocation());
		}
	}
}
