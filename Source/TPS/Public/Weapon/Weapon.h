#pragma once

#include "CoreMinimal.h"
#include "Character/TPSCharacter.h"
#include "GameFramework/Actor.h"
#include "UI/TPSHUD.h"
#include "Weapon.generated.h"

class USkeletalMeshComponent;
class USphereComponent;
class UWidgetComponent;
class UPickupComponent;
class UAnimationAsset;
class ABulletShell;

UENUM(BlueprintType)
enum class EWeaponState: uint8
{
	Ews_Initial UMETA(DisplayName = "Initial State"),
	Ews_Equipped UMETA(DisplayName = "Equipped"),
	Ews_Dropped UMETA(DisplayName = "Dropped"),
	Ews_Max UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class TPS_API AWeapon : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = Components)
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, BlueprintReadOnly, Category = Weapon,
		meta = (AllowPrivateAccess = true))
	EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, Category = Components)
	UWidgetComponent* PickupWidget;

	UPROPERTY(VisibleAnywhere, Category = Components)
	UPickupComponent* PickupComponent;

	UPROPERTY(EditAnywhere, Category = Properties)
	UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere, Category = Properties)
	TSubclassOf<ABulletShell> BulletShellClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = CrossHairs, meta=(AllowPrivateAccess = true))
	FCrossHairHUD CrossHairHUD;
	
	UPROPERTY(BlueprintReadOnly, Category = Sockets, meta = (AllowPrivateAccess = true))
	FName MuzzleSocketName = "MuzzleFlash";

	UPROPERTY(BlueprintReadOnly, Category = Sockets, meta = (AllowPrivateAccess = true))
	FName AmmoSocketName = "AmmoEject";

	UPROPERTY(EditAnywhere, Category = Properties)
	float ZoomFov = 30.f;

	UPROPERTY(EditAnywhere, Category = Properties)
	float ZoomInterpSpeed = 20.f;
	
private:

	UFUNCTION()
	void OnRep_WeaponState();

protected:
	
	UFUNCTION()
	virtual void OnPickup(ATPSCharacter* Character);

public:
	
	UFUNCTION(BlueprintCallable)
	void ShowPickupWidget(bool bShowWidget) const;

	
protected:
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:

	AWeapon();

	virtual void Fire(const FVector& HitTarget);

	void SetWeaponState(const EWeaponState State);

	FORCEINLINE FName GetMuzzleSocketName() const { return MuzzleSocketName; }

	FORCEINLINE EWeaponState GetWeaponState() const { return WeaponState; }

	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	
	FORCEINLINE FCrossHairHUD GetCrossHairHUD() const { return CrossHairHUD; }

	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	
	FORCEINLINE float GetZoomFOV() const { return ZoomFov; }
};
