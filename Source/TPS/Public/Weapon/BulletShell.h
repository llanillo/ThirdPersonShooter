#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

class UStaticMeshComponent;
class USoundCue;

UCLASS(ClassGroup=TPS)
class TPS_API ABulletShell : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Components)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Properties, meta = (AllowPrivateAccess = true))
	float EjectionImpulse = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Properties, meta = (AllowPrivateAccess = true))
	float TimeBeforeDestroy = 10.f;

	UPROPERTY(EditAnywhere, Category = Audio)
	USoundCue* DropSound;
	
	UPROPERTY()
	FTimerHandle DestroyHandle;

protected:
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	           FVector NormalImpulse, const FHitResult& Hit);

public:
	
	ABulletShell();

protected:

	virtual void BeginPlay() override;

	virtual void Destroyed() override;
	
private:
	
	void OnDestroyTimer();
};

#include "BulletShell.generated.h"