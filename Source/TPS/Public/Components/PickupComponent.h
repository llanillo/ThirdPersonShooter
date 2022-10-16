#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "PickupComponent.generated.h"

class ATPSCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickup, ATPSCharacter*, Character);

UCLASS(ClassGroup=(TPS), meta=(BlueprintSpawnableComponent))
class TPS_API UPickupComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	
	UPickupComponent();

	UPROPERTY(BlueprintAssignable,  Category = Interaction)
	FOnPickup OnPickup;

protected:
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPickupBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
							  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
							  const FHitResult& SweepResult);
};