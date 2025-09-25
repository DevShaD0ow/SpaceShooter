#include "SpaceShooterGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

ASpaceShooterGameMode::ASpaceShooterGameMode()
{
	PlayerScore = 0;
	PointsForLife = { 30, 45, 70}; 
	BasePoints = 10;
}

void ASpaceShooterGameMode::AddScore(int32 Points)
{
	PlayerScore += Points;
	UE_LOG(LogTemp, Log, TEXT("Score +%d -> %d"), Points, PlayerScore);
}

int32 ASpaceShooterGameMode::GetPointsForLife(int32 Life) const
{
	if (Life <= 0) return 0;
	int32 Index = Life - 1;
	if (PointsForLife.IsValidIndex(Index))
		return PointsForLife[Index];
	return BasePoints * Life; // fallback
}

void ASpaceShooterGameMode::GameOver()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC && GameOverWidgetClass)
	{
		UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(PC, GameOverWidgetClass);
		if (GameOverWidget)
		{
			GameOverWidget->AddToViewport();

			PC->bShowMouseCursor = true;
			PC->SetInputMode(FInputModeUIOnly());

			UE_LOG(LogTemp, Warning, TEXT("GameOver widget affiché"));
		}
	}
}
