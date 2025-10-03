#include "SpawnZone.h"
#include "Asteroide.h"
#include "EngineUtils.h"
#include "Vaisseau.h"
#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ASpawnZone::ASpawnZone()
{
    PrimaryActorTick.bCanEverTick = true;

    SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
    RootComponent = SpawnVolume;

    // Configuration collision pour overlap
    SpawnVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SpawnVolume->SetCollisionObjectType(ECC_WorldStatic);
    SpawnVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
    SpawnVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);       // vaisseau
    SpawnVolume->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap); // astéroïdes / projectiles
    SpawnVolume->SetGenerateOverlapEvents(true);
}

void ASpawnZone::BeginPlay()
{
    Super::BeginPlay();
    if (SpawnVolume)
    {
        SpawnVolume->OnComponentEndOverlap.AddDynamic(this, &ASpawnZone::OnEndOverlap);
    }

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

    int32 CurrentAsteroids = 0;
    for (TActorIterator<AAsteroide> It(World); It; ++It)
    {
        if (It->OwnerSpawnZone == this)
            CurrentAsteroids++;
    }

    if (CurrentAsteroids >= NumberOfAsteroids) return;

    FVector SpawnLocation = GetRandomPointInVolume();
    FRotator SpawnRotation = FRotator::ZeroRotator;

    AAsteroide* NewAsteroid = World->SpawnActor<AAsteroide>(AsteroidClass, SpawnLocation, SpawnRotation);
    if (NewAsteroid)NewAsteroid->OwnerSpawnZone = this;
}


void ASpawnZone::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!OtherActor) return;
    if (AVaisseau* Vaisseau = Cast<AVaisseau>(OtherActor))
    {
        Vaisseau->PerdreVie();

        FVector RespawnLocation(23610.f, -21180.f, 320.f);
        Vaisseau->SetActorLocation(RespawnLocation);
        Vaisseau->SetActorRotation(FRotator::ZeroRotator);
    }
    else if (AAsteroide* Asteroide = Cast<AAsteroide>(OtherActor))
    {
        Asteroide->DestroyAsteroide();
        RespawnAsteroid();
    }
    else if (AProjectile* Projectile = Cast<AProjectile>(OtherActor))Projectile->Destroy();
}
