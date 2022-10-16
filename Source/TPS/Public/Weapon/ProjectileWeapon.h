#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ProjectileWeapon.generated.h"

class UBoxComponent;
class AProjectile;

UCLASS(ClassGroup=TPS)
class TPS_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Properties)
	TSubclassOf<AProjectile> ProjectileClass;
	
	UPROPERTY(EditAnywhere, Category = Components)
	UBoxComponent* CollisionBox;
	
public:
	
	AProjectileWeapon();

	virtual void Fire(const FVector& HitTarget) override;
	
protected:
	
	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;
};
