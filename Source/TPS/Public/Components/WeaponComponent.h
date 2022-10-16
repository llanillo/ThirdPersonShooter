#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

#define TRACE_LENGTH 80000.f

class AWeapon;
class ATPSCharacter;

UCLASS(ClassGroup=(TPS), meta=(BlueprintSpawnableComponent))
class TPS_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = true))
	FName CharacterHandSocket;

	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_WeaponEquipped, Category = Weapon)
	AWeapon* WeaponEquipped;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = Weapon, meta = (AllowPrivateAccess = true))
	bool bIsAiming;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = Weapon, meta = (AllowPrivateAccess = true))
	bool bIsFiring;
	
	UPROPERTY()
	ATPSCharacter* Character;

protected:

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastFire(const FVector_NetQuantize& TraceTargetHit);
	
	UFUNCTION(Server, Reliable)
	virtual void ServerFire(const FVector_NetQuantize& TraceTargetHit);
	
public:

	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual void BeginFire(bool bIsShooting);
	
	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual void EndFire();

private:

	UFUNCTION()
	void TraceLineFromCrosshair(FHitResult& OutHitResult);
	
	UFUNCTION(Server, Reliable)
	void ServerStartAiming(bool bAiming);

	UFUNCTION()
	void OnRep_WeaponEquipped(AWeapon* LastWeapon);

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

	FORCEINLINE bool IsWeaponEquipped() const { return WeaponEquipped != nullptr; }
	
	FORCEINLINE bool IsAiming() const { return bIsAiming; }
	
	FORCEINLINE AWeapon* GetWeaponEquipped() const { return WeaponEquipped; }

};
