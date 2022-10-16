#include "Weapon/BulletShell.h"

ABulletShell::ABulletShell()
{
	ShellMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	ShellMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	ShellMeshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	SetRootComponent(ShellMeshComponent);
}

void ABulletShell::BeginPlay()
{
	Super::BeginPlay();
	
}


