#pragma once

#include "CoreMinimal.h"
#include "TPSPlayerController.generated.h"

UCLASS(ClassGroup=TPS)
class TPS_API ATPSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATPSPlayerController();

protected:

	virtual void BeginPlay() override;

public:
	
	virtual void Tick(float DeltaTime) override;
};
