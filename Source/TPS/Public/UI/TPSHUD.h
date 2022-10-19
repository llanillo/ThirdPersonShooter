#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TPSHUD.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct FCrossHairHUD
{
	GENERATED_BODY();
	
	UTexture2D* CrossHairCenter;
	UTexture2D* CrossHairTop;
	UTexture2D* CrossHairBottom;
	UTexture2D* CrossHairLeft;
	UTexture2D* CrossHairRight;
};

UCLASS(ClassGroup=TPS)
class TPS_API ATPSHUD : public AHUD
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Weapon)
	FCrossHairHUD CrossHairHUD;

public:

	ATPSHUD();

protected:

	virtual void BeginPlay() override;

	virtual void DrawHUD() override;

private:

	void DrawWeaponCrossHair(const FVector2D ViewportSize);

	void DrawCrossHair(const FVector2D Position, UTexture2D* Texture);
	
public:

	virtual void Tick(float DeltaTime) override;

	FORCEINLINE void SetCrossHairHUD(const FCrossHairHUD& NewCrossHairHUD) { CrossHairHUD = NewCrossHairHUD; }
};
