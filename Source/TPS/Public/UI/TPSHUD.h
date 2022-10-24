#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TPSHUD.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct FCrossHairHUD
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, Category = CrossHair)
	UTexture2D* CrossHairCenter;
	UPROPERTY(EditAnywhere, Category = CrossHair)
	UTexture2D* CrossHairTop;
	UPROPERTY(EditAnywhere, Category = CrossHair)
	UTexture2D* CrossHairBottom;
	UPROPERTY(EditAnywhere, Category = CrossHair)
	UTexture2D* CrossHairLeft;
	UPROPERTY(EditAnywhere, Category = CrossHair)
	UTexture2D* CrossHairRight;

	float Spread;
};

UCLASS(ClassGroup=TPS)
class TPS_API ATPSHUD : public AHUD
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Weapon)
	FCrossHairHUD CrossHairHUD;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spread, meta = (AllowPrivateAccess = true))
	float CrossHairSpreadMax = 16.f;

public:

	ATPSHUD();

protected:

	virtual void BeginPlay() override;

	virtual void DrawHUD() override;

private:

	void DrawWeaponCrossHair(const FVector2D ViewportSize);

	void DrawCrossHair(const FVector2D Position, UTexture2D* Texture, FVector2D Spread);
	
public:

	virtual void Tick(float DeltaTime) override;

	FORCEINLINE void SetCrossHairHUD(const FCrossHairHUD& NewCrossHairHUD) { CrossHairHUD = NewCrossHairHUD; }
};
