#include "Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

// cette ligne est pas inutile serieux
// ajout de cette ligne inutile pour le conflit =)

AProjectile::AProjectile()
{
  PrimaryActorTick.bCanEverTick = true;
  
  ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
  RootComponent = ProjectileMesh;
  
  ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
  ProjectileMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
  ProjectileMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap); 
  ProjectileMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);         
}

void AProjectile::BeginPlay()
{
  Super::BeginPlay();

  if (FireSound)UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
  
}

void AProjectile::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
  FVector NewLocation = GetActorLocation() + (GetActorForwardVector() * Speed * DeltaTime);
  SetActorLocation(NewLocation);
}
