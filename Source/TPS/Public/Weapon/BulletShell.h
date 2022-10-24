#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletShell.generated.h"

class USoundCue;

UCLASS(ClassGroup=TPS)
class TPS_API ABulletShell : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Components)
	UStaticMeshComponent* ShellMeshComponent;

	UPROPERTY(EditAnywhere, Category = Properties)
	USoundCue* DropSound;

	UPROPERTY()
	FTimerHandle DestroyHandle;
	
	UPROPERTY(EditAnywhere, Category = Properties)
	float EjectionImpulse = 5.f;

	UPROPERTY(EditAnywhere, Category = Properties)
	float BulletLifeSpan = 15.f;

	
protected:

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                   FVector NormalImpulse, const FHitResult& Hit);

private:

	bool bAlreadyPlayedSound = false;
	
public:
	
	ABulletShell();

protected:
	
	virtual void BeginPlay() override;
};
