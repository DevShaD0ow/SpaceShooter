#include "Projectile.h"
#include "Components/StaticMeshComponent.h"

// cette ligne est pas inutile serieux
// ajout de cette ligne inutile pour le conflit =)

AProjectile::AProjectile()
{
  PrimaryActorTick.bCanEverTick = true;
  
  ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
  RootComponent = ProjectileMesh;
  
  // Collision : le projectile "overlap" seulement
  ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
  ProjectileMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
  ProjectileMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap); // astéroïdes
  ProjectileMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);         // vaisseau / ennemis
}

void AProjectile::BeginPlay()
{
  Super::BeginPlay();
}

void AProjectile::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
  
  // Avance tout droit
  FVector NewLocation = GetActorLocation() + (GetActorForwardVector() * Speed * DeltaTime);
  SetActorLocation(NewLocation);
}
