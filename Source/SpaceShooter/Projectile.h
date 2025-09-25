#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class SPACESHOOTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// Mesh du projectile
	UPROPERTY(VisibleAnywhere, Category="Components")
	class UStaticMeshComponent* ProjectileMesh;

	// Vitesse
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float Speed = 2000.0f;
};
