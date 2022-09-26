#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class ATWeapon;
class UCombatComponent;

UCLASS(ClassGroup=TPS)
class TPS_API ATPSCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = Components)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Components)
	UCameraComponent* FollowCamera;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon)
	ATWeapon* CurrentWeapon;

	UPROPERTY(VisibleAnywhere, Category = Components)
	UCombatComponent* CombatComponent;
	
protected:
public:
private:

	UFUNCTION()
	void OnRep_CurrentWeapon(ATWeapon* LastWeapon);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEquipWeapon();
	
protected:
public:

private:
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void MoveForward(float Value);
	
	virtual void MoveRight(float Value);
	
	virtual void Turn(float Value);
	
	virtual void LookUp(float Value);

	virtual void BeginCrouch();

	virtual void EndCrouch();

	virtual void BeginAim();

	virtual void EndAim();

	virtual void EquipWeapon();
	
public:
	
	// Sets default values for this character's properties
	ATPSCharacter();
	
	void SetCurrentWeapon(ATWeapon* Weapon);
	
	FORCEINLINE ATWeapon* GetCurrentWeapon() const { return CurrentWeapon; }

	bool IsWeaponEquipped() const;

	bool IsAiming() const;
};
