#pragma once

#include "CoreMinimal.h"
#include "UI/TPSHUD.h"
#include "WeaponComponent.generated.h"

#define TRACE_LENGTH 80000.f

class AWeapon;
class ATPSCharacter;
class ATPSPlayerController;

UCLASS(ClassGroup=(TPS), meta=(BlueprintSpawnableComponent))
class TPS_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = true))
	FName CharacterHandSocket;

	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_WeaponEquipped, Category = Weapon)
	AWeapon* WeaponEquipped;

	UPROPERTY(BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
	FVector HitTarget;
	
	UPROPERTY(BlueprintReadOnly, Replicated, Category = Weapon, meta = (AllowPrivateAccess = true))
	bool bIsAiming;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = Weapon, meta = (AllowPrivateAccess = true))
	bool bIsFiring;
	
	UPROPERTY()
	ATPSCharacter* Character;

	UPROPERTY()
	ATPSPlayerController* PlayerController;

	UPROPERTY()
	ATPSHUD* PlayerHUD;

private:
	
	UFUNCTION()
	void TraceLineFromCrosshair(FHitResult& OutHitResult);
	
	UFUNCTION(Server, Reliable)
	void ServerStartAiming(bool bAiming);

	UFUNCTION()
	void OnRep_WeaponEquipped(AWeapon* LastWeapon);

protected:

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastFire(const FVector_NetQuantize& TraceTargetHit);
	
	UFUNCTION(Server, Reliable)
	virtual void ServerFire(const FVector_NetQuantize& TraceTargetHit);

	virtual void SetCrossHairsHUD(float DeltaTime);
	
public:

	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual void BeginFire(bool bIsShooting);
	
	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual void EndFire();

private:

	float CrossHairAirFactor;
	
	float DefaultFOV;
	
	float CurrentFOV;

	void InterpFOV(float DeltaTime);

protected:
	
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	
	UWeaponComponent();

	void EquipWeapon(AWeapon* Weapon);

	void StartAiming(bool bAiming);

	FORCEINLINE void SetCharacter(ATPSCharacter* NewCharacter) { Character = NewCharacter; }
	
	FORCEINLINE void SetCharacterController(ATPSPlayerController* Controller) { PlayerController = Controller; }

	FORCEINLINE void SetCharacterHUD(ATPSHUD* HUD) { PlayerHUD = HUD; }

	FORCEINLINE bool IsWeaponEquipped() const { return WeaponEquipped != nullptr; }
	
	FORCEINLINE bool IsAiming() const { return bIsAiming; }
	
	FORCEINLINE AWeapon* GetWeaponEquipped() const { return WeaponEquipped; }

	FORCEINLINE const FVector& GetHitTarget() const { return HitTarget; }

};
