#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class ATWeapon;
class ATPSCharacter;

UCLASS(ClassGroup=(TPS), meta=(BlueprintSpawnableComponent))
class TPS_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = true))
	FName CharacterSocket;

	UPROPERTY(VisibleInstanceOnly, Replicated, Category = Weapon)
	ATWeapon* WeaponEquipped;

	UPROPERTY(VisibleInstanceOnly, Category = Character)
	ATPSCharacter* Character;

	UPROPERTY(VisibleInstanceOnly, Replicated, Category = Weapon)
	bool bIsAiming;

protected:
public:
private:

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetAiming(bool bAiming);

protected:
	
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	
	// friend class ATPSCharacter;

	UCombatComponent();

	void EquipWeapon(ATWeapon* WeaponToEquip);

	void SetAiming(bool bAiming);

	void SetCharacter(ATPSCharacter* NewCharacter);

	FORCEINLINE bool IsAiming() const { return bIsAiming; }
	
	FORCEINLINE const ATWeapon* GetWeaponEquipped() const { return WeaponEquipped; }
};
