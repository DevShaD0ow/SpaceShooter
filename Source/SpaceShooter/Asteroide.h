#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Asteroide.generated.h"

class UNiagaraSystem;
class ASpawnZone;

UCLASS()
class SPACESHOOTER_API AAsteroide : public AActor
{
	GENERATED_BODY()
	
public:	
	AAsteroide();
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	class UStaticMeshComponent* AsteroideMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	FVector Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float Speed = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	FRotator RotationSpeed;

	UPROPERTY(EditAnywhere, Category = "Effets")
	UParticleSystem* ExplosionFx;
	UPROPERTY(EditAnywhere, Category = "Effets")
	UParticleSystem* ImpactFX;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* BoxCollider;
	
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,
				   UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,
				   bool bFromSweep,const FHitResult& SweepResult);
	void DestroyAsteroide();

	UPROPERTY()
	ASpawnZone* OwnerSpawnZone;

private:
	int vie;
	int InitialVie;
};
