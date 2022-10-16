#include "Components/PickupComponent.h"
#include "Character/TPSCharacter.h"
#include "Weapon/Weapon.h"

UPickupComponent::UPickupComponent()
{
	UPrimitiveComponent::SetCollisionResponseToAllChannels(ECR_Ignore);
	UPrimitiveComponent::SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereRadius = 45.f;
}


void UPickupComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const AActor* Owner = Cast<AActor>(GetOwner()))
	{
		if (!Owner->HasAuthority()) return;

		SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnPickupBeginOverlap);
	}
}

void UPickupComponent::OnPickupBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                            const FHitResult& SweepResult)
{
	if (ATPSCharacter* Player = Cast<ATPSCharacter>(OtherActor))
	{
		OnPickup.Broadcast(Player);
		OnComponentBeginOverlap.RemoveAll(this);
		SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}