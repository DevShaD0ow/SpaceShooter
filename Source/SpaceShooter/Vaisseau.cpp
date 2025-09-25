#include "Vaisseau.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "SpaceShooterGameMode.h"

//Super jeu !

AVaisseau::AVaisseau()
{
	PrimaryActorTick.bCanEverTick = true;

	// Racine
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Mesh
	VaisseauMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VaisseauMesh"));
	VaisseauMesh->SetupAttachment(RootComponent);

	// Possession auto par le joueur 1
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void AVaisseau::BeginPlay()
{
	Super::BeginPlay();
}

void AVaisseau::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// --- Déplacement relatif à l'orientation ---
	if (!CurrentInput.IsNearlyZero())
	{
		FVector Forward = GetActorForwardVector();
		FVector Right = GetActorRightVector();

		FVector Direction = (Forward * CurrentInput.X) + (Right * CurrentInput.Y);

		if (!Direction.IsNearlyZero())
		{
			FVector NewLocation = GetActorLocation() + (Direction.GetSafeNormal() * MoveSpeed * DeltaTime);
			SetActorLocation(NewLocation);
		}
	}
}

void AVaisseau::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AVaisseau::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AVaisseau::MoveRight);
	PlayerInputComponent->BindAxis("Shoot", this, &AVaisseau::Shoot);

	// Souris (rotation)
	PlayerInputComponent->BindAxis("Turn", this, &AVaisseau::Turn);
}

void AVaisseau::MoveForward(float Value)
{
	CurrentInput.X = Value;
}

void AVaisseau::MoveRight(float Value)
{
	CurrentInput.Y = Value;
}

void AVaisseau::Turn(float Value)
{
	if (FMath::Abs(Value) > KINDA_SMALL_NUMBER)
	{
		FRotator NewRotation = GetActorRotation();
		NewRotation.Yaw += Value;
		SetActorRotation(NewRotation);
	}
}

void AVaisseau::Shoot(float Value)
{
	if (Value > 0.0f && bCanShoot && ProjectileClass)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
			FRotator SpawnRotation = GetActorRotation();

			World->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);

			bCanShoot = false;
			World->GetTimerManager().SetTimer(FireRateTimerHandle, this, &AVaisseau::ResetCanShoot, FireCooldown, false);
		}
	}
}

void AVaisseau::ResetCanShoot()
{
	bCanShoot = true;
}

void AVaisseau::PerdreVie()
{
	if (bIsInvincible) return;

	if (ExplosionFX)
	{
		FRotator ExplosionRotation = GetActorRotation();
		ExplosionRotation.Yaw += 90.f;

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ExplosionFX,
			GetActorLocation(),
			ExplosionRotation,
			FVector(7.f)
		);
	}

	// Retire une vie
	NbVies--;

	// Active l’invincibilité temporaire
	bIsInvincible = true;
	GetWorld()->GetTimerManager().SetTimer(
		InvincibilityTimerHandle,
		this,
		&AVaisseau::ResetInvincibility,
		InvincibilityDuration,
		false
	);

	// 👉 Vérifie si le joueur est mort
	if (NbVies <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Plus de vies, signal GameOver"));

		// Désactiver le vaisseau
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		SetActorTickEnabled(false);

		// Prévenir le GameMode
		ASpaceShooterGameMode* GM = Cast<ASpaceShooterGameMode>(UGameplayStatics::GetGameMode(this));
		if (GM)
		{
			GM->GameOver();
		}
	}
}

void AVaisseau::ResetInvincibility()
{
	bIsInvincible = false;
}
