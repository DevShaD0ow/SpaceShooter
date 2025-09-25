#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SpaceShooterGameMode.generated.h"

UCLASS()
class SPACESHOOTER_API ASpaceShooterGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASpaceShooterGameMode();

	// Score global du joueur
	UPROPERTY(BlueprintReadOnly, Category="Score")
	int32 PlayerScore;

	// Tableau éditable depuis l’éditeur :
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Score")
	TArray<int32> PointsForLife;

	// Valeur par défaut si PointsForLife n’a pas assez d’entrées
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Score")
	int32 BasePoints = 10;

	// Ajouter des points
	UFUNCTION(BlueprintCallable, Category="Score")
	void AddScore(int32 Points);

	UFUNCTION(BlueprintCallable, Category="Score")
	int32 GetScore() const { return PlayerScore; }

	// Récupérer le nombre de points pour une astéroïde en fonction de sa vie initiale
	UFUNCTION(BlueprintCallable, Category="Score")
	int32 GetPointsForLife(int32 Life) const;

	// --- Game Over ---
	UFUNCTION(BlueprintCallable, Category="Game")
	void GameOver();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	TSubclassOf<class UUserWidget> GameOverWidgetClass;
};
