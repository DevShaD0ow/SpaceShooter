#include "Asteroide.h"
#include "Vaisseau.h"
#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "SpaceShooterGameMode.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"	
#include "Kismet/GameplayStatics.h"
#include "SpawnZone.h"

// Sets default values
AAsteroide::AAsteroide()
{
	PrimaryActorTick.bCanEverTick = true;

	// Root
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Mesh
	AsteroideMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AsteroideMesh"));
	AsteroideMesh->SetupAttachment(RootComponent);

	// Collision
	AsteroideMesh->SetSimulatePhysics(false); 
	AsteroideMesh->SetEnableGravity(false);   
	AsteroideMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AsteroideMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	AsteroideMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	AsteroideMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

	// Box Collider pour gérer les overlaps
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	BoxCollider->SetupAttachment(RootComponent);
	BoxCollider->SetBoxExtent(FVector(50.f));
	BoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollider->SetGenerateOverlapEvents(true);
	BoxCollider->SetCollisionObjectType(ECC_WorldDynamic);
	BoxCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxCollider->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	BoxCollider->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

	// Bind overlap
	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &AAsteroide::OnOverlap);

	// Vie aléatoire
	vie = FMath::RandRange(1, 3);
	InitialVie = vie;
}

void AAsteroide::BeginPlay()
{
	Super::BeginPlay();

	// Direction aléatoire
	Direction = FMath::VRand().GetSafeNormal();

	// Rotation aléatoire sur Z
	RotationSpeed = FRotator(0.f, FMath::FRandRange(-45.f, 45.f), 0.f);

	// Taille aléatoire
	float RandomScale = FMath::FRandRange(0.5f, 2.0f);
	SetActorScale3D(FVector(RandomScale));
}

void AAsteroide::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);

	// Déplacement
	FVector Location = GetActorLocation();
	Location.X += Direction.X * Speed * DeltaTime;
	Location.Y += Direction.Y * Speed * DeltaTime;
	SetActorLocation(Location);

	// Rotation Z
	FRotator NewRotation = FRotator(0.f, RotationSpeed.Yaw * DeltaTime, 0.f);
	AddActorLocalRotation(NewRotation);
}

void AAsteroide::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && OtherComp)
	{
		// Projectile
		AProjectile* Projectile = Cast<AProjectile>(OtherActor);
		if (Projectile)
		{
			vie--;

			if (vie <= 0)
			{
				if (ASpaceShooterGameMode* GM = Cast<ASpaceShooterGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
				{
					int32 PointsToAdd = GM->GetPointsForLife(InitialVie);
					GM->AddScore(PointsToAdd);
				}

				UE_LOG(LogTemp, Warning, TEXT("Astéroïde détruit par projectile"));

				// ✅ Respawn via la zone
				if (OwnerSpawnZone)
				{
					OwnerSpawnZone->RespawnAsteroid();
				}

				Projectile->Destroy();
				Destroy();
			}
			else
			{
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
				Projectile->Destroy();
			}
		}

		// Vaisseau
		AVaisseau* Vaisseau = Cast<AVaisseau>(OtherActor);
		if (Vaisseau)
		{
			UE_LOG(LogTemp, Warning, TEXT("Astéroïde a touché le VAISSEAU"));
			Vaisseau->PerdreVie();
		}
	}
}
