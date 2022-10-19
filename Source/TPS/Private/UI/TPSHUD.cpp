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

	if(!GEngine) return;

	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

}

void ATPSHUD::DrawWeaponCrossHair(const FVector2D ViewportSize)
{
	const FVector2D ViewportCenter {ViewportSize.X / 2.f, ViewportSize.Y / 2.f};
	if(CrossHairHUD.CrossHairCenter)
	{
		DrawCrossHair(ViewportCenter, CrossHairHUD.CrossHairCenter);
	}
	
	if(CrossHairHUD.CrossHairTop)
	{
		DrawCrossHair(ViewportCenter, CrossHairHUD.CrossHairTop);
	}
	if(CrossHairHUD.CrossHairBottom)
	{
		DrawCrossHair(ViewportCenter, CrossHairHUD.CrossHairBottom);
	}
	
	if(CrossHairHUD.CrossHairLeft)
	{
		DrawCrossHair(ViewportCenter, CrossHairHUD.CrossHairLeft);
	}

	if(CrossHairHUD.CrossHairRight)
	{
		DrawCrossHair(ViewportCenter, CrossHairHUD.CrossHairRight);
	}
}

void ATPSHUD::DrawCrossHair(const FVector2D Position, UTexture2D* Texture)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D DrawPosition {Position.X - TextureWidth / 2.f, Position.Y - TextureHeight / 2.f};
	DrawTexture(Texture, DrawPosition.X, DrawPosition.Y, TextureWidth, TextureHeight, 0.f, 0.f, 1, 1);
}

