#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Projectile.h"
#include "NiagaraSystem.h"
#include "Vaisseau.generated.h"

UCLASS()
class SPACESHOOTER_API AVaisseau : public APawn
{
	GENERATED_BODY()

public:
	AVaisseau();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	class UStaticMeshComponent* VaisseauMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vie")
	int NbVies = 3;

	// Effet d’explosion
	UPROPERTY(EditAnywhere, BlueprintReadWrite ,Category="Explosion")
	UNiagaraSystem* ExplosionFX; 

	// Fonction appelée quand le vaisseau perd une vie
	UFUNCTION(BlueprintCallable, Category="Vie")
	void PerdreVie();

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void Shoot(float Value);
	void ResetCanShoot();
	FVector CurrentInput;

	bool bCanShoot = true;         
	float FireCooldown = 0.5f;  
	FTimerHandle FireRateTimerHandle;  

	bool bIsInvincible = false;
	FTimerHandle InvincibilityTimerHandle;
	float InvincibilityDuration = 0.5f; 
	void ResetInvincibility();
};
