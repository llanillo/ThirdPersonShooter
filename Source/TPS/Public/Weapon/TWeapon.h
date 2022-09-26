#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TWeapon.generated.h"

class USkeletalMeshComponent;
class USphereComponent;
class UWidgetComponent;

UENUM(BlueprintType)
enum class EWeaponState: uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),
	EWS_Max UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class TPS_API ATWeapon : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = Components)
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = Components)
	USphereComponent* SphereArea;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, BlueprintReadOnly, Category = Properties, meta = (AllowPrivateAccess = true))
	EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, Category = Properties)
	UWidgetComponent* PickupWidget;

protected:
public:

private:
	 
	UFUNCTION()
	virtual void OnSphereAreaOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                 const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnRep_WeaponState();
	
protected:
public:
	
	UFUNCTION(BlueprintCallable)
	void ShowPickupWidget(bool bShowWidget) const;

private:
protected:
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	
	// Sets default values for this actor's properties
	ATWeapon();

	FORCEINLINE void SetWeaponState(const EWeaponState State);

	FORCEINLINE USphereComponent* GetSphereArea() const { return SphereArea; }
};
