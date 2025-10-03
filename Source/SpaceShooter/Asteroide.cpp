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

	// Box Collider
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

	// Récupère le vaisseau
	AVaisseau* Vaisseau = Cast<AVaisseau>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (Vaisseau)
	{
		FVector DirectionToVaisseau = (Vaisseau->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		Direction = DirectionToVaisseau;
	}
	else Direction = FMath::VRand().GetSafeNormal();
	RotationSpeed = FRotator(0.f, FMath::FRandRange(-45.f, 45.f), 0.f);

	// Taille aléatoire
	float RandomScale = FMath::FRandRange(0.5f, 2.0f);
	SetActorScale3D(FVector(RandomScale));
}


void AAsteroide::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);

	AVaisseau* Vaisseau = Cast<AVaisseau>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (Vaisseau)
	{
		Direction = (Vaisseau->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	}

	// Déplacement vers la cible
	FVector Location = GetActorLocation();
	Location += Direction * Speed * DeltaTime;
	SetActorLocation(Location);


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
			if (ImpactFX)
			{
				FVector ImpactLocation = GetActorLocation();
				if (SweepResult.bBlockingHit)
				{
					ImpactLocation = FVector(SweepResult.ImpactPoint);
				}

				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactFX,
					ImpactLocation,
					SweepResult.ImpactNormal.Rotation(),
					FVector(3.0f),
					true
				);
			}
			// Vérif destruction
			if (vie <= 0)
			{
				if (ASpaceShooterGameMode* GM = Cast<ASpaceShooterGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
				{
					int32 PointsToAdd = GM->GetPointsForLife(InitialVie);
					GM->AddScore(PointsToAdd);
				}

				if (ExplosionFx)
				{
					FRotator ImpactRotation = SweepResult.ImpactNormal.Rotation();
					ImpactRotation.Pitch += 90.0f;

					UGameplayStatics::SpawnEmitterAtLocation(
						GetWorld(),
						ExplosionFx,
						GetActorLocation(),
						ImpactRotation,
						FVector(3.0f),
						true
					);

				}
				Destroy();
				if (OwnerSpawnZone)
				{
					OwnerSpawnZone->RespawnAsteroid();
				}
			}

			Projectile->Destroy();
		}

		// Vaisseau
		AVaisseau* Vaisseau = Cast<AVaisseau>(OtherActor);
		if (Vaisseau)
		{
			Vaisseau->PerdreVie();
			Destroy();
		}
	}
}

void AAsteroide::DestroyAsteroide()
{
	Destroy();
}
