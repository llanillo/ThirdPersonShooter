#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "Projectile.generated.h"

class UBoxComponent;
class UProjectileMovementComponent;
class UParticleSystem;

UCLASS(ClassGroup=TPS)
class TPS_API AProjectile : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Componentes)
	UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere, Category = Components)
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, Category = Properties)
	UParticleSystem* TracerParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Properties, meta = (AllowPrivateAccess = true))
	USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Properties, meta = (AllowPrivateAccess = true))
	UParticleSystem* ImpactParticleSystem;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Properties, meta = (AllowPrivateAccess = true))
	float ProjectileSpeed = 15000.f;

protected:
	
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                   FVector NormalImpulse, const FHitResult& Hit);
public:
	
	AProjectile();

protected:
	
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

public:
	virtual void Tick(float DeltaTime) override;
};
