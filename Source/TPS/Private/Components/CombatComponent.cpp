#include "Components/CombatComponent.h"
#include "Weapon/TWeapon.h"
#include "Character/TPSCharacter.h"
#include "Net/UnrealNetwork.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CharacterSocket = "GripPoint";
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, WeaponEquipped);
	DOREPLIFETIME(UCombatComponent, bIsAiming);
}

void UCombatComponent::EquipWeapon(ATWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr)
	{
		return;
	}

	WeaponEquipped = WeaponToEquip;
	WeaponEquipped->SetWeaponState(EWeaponState::EWS_Equipped);
	WeaponEquipped->AttachToActor(Character, FAttachmentTransformRules::SnapToTargetNotIncludingScale, CharacterSocket);
	WeaponEquipped->SetOwner(Character);
}

void UCombatComponent::SetCharacter(ATPSCharacter* NewCharacter)
{
	Character = NewCharacter;
}

void UCombatComponent::SetAiming(bool bAiming)
{
	bIsAiming = bAiming;
	ServerSetAiming(bAiming);
}

void UCombatComponent::ServerSetAiming_Implementation(bool bAiming)
{
	bIsAiming = bAiming;
}

bool UCombatComponent::ServerSetAiming_Validate(bool bAiming)
{
	return true;
}
