#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnZone.generated.h"

class UBoxComponent;
class AAsteroide;

UCLASS()
class SPACESHOOTER_API ASpawnZone : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpawnZone();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// Box pour définir la zone de spawn
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* SpawnVolume;

	// Classe d’astéroïde à spawn
	UPROPERTY(EditAnywhere, Category="Spawn")
	TSubclassOf<AAsteroide> AsteroidClass;

	// Nombre d’astéroïdes au départ
	UPROPERTY(EditAnywhere, Category="Spawn")
	int32 NumberOfAsteroids = 12;

	// Spawn initial
	void SpawnAsteroids();

	// Respawn d’un seul astéroïde
	void RespawnAsteroid();

	// Position aléatoire dans le volume
	FVector GetRandomPointInVolume() const;

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
