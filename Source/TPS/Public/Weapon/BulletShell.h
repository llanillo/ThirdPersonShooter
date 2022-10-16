#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletShell.generated.h"

UCLASS(ClassGroup=TPS)
class TPS_API ABulletShell : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Components)
	UStaticMeshComponent* ShellMeshComponent;
	
public:
	
	ABulletShell();

protected:

	virtual void BeginPlay() override;

public:
};
