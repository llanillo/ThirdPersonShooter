#include "UI/TPSHUD.h"
#include "Engine/Texture2D.h"

ATPSHUD::ATPSHUD()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATPSHUD::BeginPlay()
{
	Super::BeginPlay();
}

void ATPSHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATPSHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!GEngine) return;

	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	DrawWeaponCrossHair(ViewportSize);
}

void ATPSHUD::DrawWeaponCrossHair(const FVector2D ViewportSize)
{
	const FVector2D ViewportCenter{ViewportSize.X / 2.f, ViewportSize.Y / 2.f};
	const float SpreadScaled = CrossHairSpreadMax * CrossHairHUD.Spread;

	if (CrossHairHUD.CrossHairCenter)
	{
		const FVector2D CenterSpread{0.f, 0.f};
		DrawCrossHair(ViewportCenter, CrossHairHUD.CrossHairCenter, CenterSpread);
	}

	if (CrossHairHUD.CrossHairTop)
	{
		const FVector2D TopSpread{0.f, -SpreadScaled};
		DrawCrossHair(ViewportCenter, CrossHairHUD.CrossHairTop, TopSpread);
	}
	if (CrossHairHUD.CrossHairBottom)
	{
		const FVector2D BottomSpread{0.f, SpreadScaled};
		DrawCrossHair(ViewportCenter, CrossHairHUD.CrossHairBottom, BottomSpread);
	}

	if (CrossHairHUD.CrossHairLeft)
	{
		const FVector2D LeftSpread{-SpreadScaled, 0.f};
		DrawCrossHair(ViewportCenter, CrossHairHUD.CrossHairLeft, LeftSpread);
	}

	if (CrossHairHUD.CrossHairRight)
	{
		const FVector2D RightSpread{SpreadScaled, 0.f};
		DrawCrossHair(ViewportCenter, CrossHairHUD.CrossHairRight, RightSpread);
	}
}

void ATPSHUD::DrawCrossHair(const FVector2D Position, UTexture2D* Texture, FVector2D Spread)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D DrawPosition{
		Position.X - TextureWidth / 2.f + Spread.X, Position.Y - TextureHeight / 2.f + Spread.Y
	};
	DrawTexture(Texture, DrawPosition.X, DrawPosition.Y, TextureWidth, TextureHeight, 0.f, 0.f, 1, 1);
}
