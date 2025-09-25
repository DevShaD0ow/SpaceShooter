#include "SpawnZone.h"
#include "Asteroide.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"

ASpawnZone::ASpawnZone()
{
	PrimaryActorTick.bCanEverTick = true;

	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	RootComponent = SpawnVolume;
}

void ASpawnZone::BeginPlay()
{
	Super::BeginPlay();
	SpawnAsteroids();
}

void ASpawnZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector ASpawnZone::GetRandomPointInVolume() const
{
	if (!SpawnVolume) return FVector::ZeroVector;

	FVector Origin = SpawnVolume->Bounds.Origin;
	FVector Extent = SpawnVolume->Bounds.BoxExtent;

	float RandX = FMath::FRandRange(-Extent.X, Extent.X);
	float RandY = FMath::FRandRange(-Extent.Y, Extent.Y);
	float Z = Origin.Z;

	return FVector(Origin.X + RandX, Origin.Y + RandY, Z);
}

void ASpawnZone::SpawnAsteroids()
{
	if (!AsteroidClass) return;
	UWorld* World = GetWorld();
	if (!World) return;

	for (int32 i = 0; i < NumberOfAsteroids; i++)
	{
		FVector SpawnLocation = GetRandomPointInVolume();
		FRotator SpawnRotation = FRotator::ZeroRotator;
		AAsteroide* NewAsteroid = World->SpawnActor<AAsteroide>(AsteroidClass, SpawnLocation, SpawnRotation);
		if (NewAsteroid)
		{
			NewAsteroid->OwnerSpawnZone = this;
		}
	}
}

void ASpawnZone::RespawnAsteroid()
{
	if (!AsteroidClass) return;
	UWorld* World = GetWorld();
	if (!World) return;

	FVector SpawnLocation = GetRandomPointInVolume();
	FRotator SpawnRotation = FRotator::ZeroRotator;

	AAsteroide* NewAsteroid = World->SpawnActor<AAsteroide>(AsteroidClass, SpawnLocation, SpawnRotation);
	if (NewAsteroid)
	{
		NewAsteroid->OwnerSpawnZone = this;
	}
}
