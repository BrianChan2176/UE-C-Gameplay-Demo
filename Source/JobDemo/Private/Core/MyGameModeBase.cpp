// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MyGameModeBase.h"
#include "Kismet/Gameplaystatics.h"
#include "AI/EnemyCharacter.h"
#include "Core/MyPlayerController.h"
void AMyGameModeBase::NotifyEnemyDied()
{
	if (bGameFinished) { return; }

	RemainingEnemies = RemainingEnemies - 1;
	UE_LOG(LogTemp, Display, TEXT("Enemy Count: %d"), RemainingEnemies);

	if (RemainingEnemies <= 0)
	{
		bGameFinished = true;
		AMyPlayerController* PlayerController=Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
		if (PlayerController)
		{
			PlayerController->Victory();
		}
	}
}

void AMyGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*>Enemies;
	UGameplayStatics::GetAllActorsOfClass(this, AEnemyCharacter::StaticClass(), Enemies);
	RemainingEnemies = Enemies.Num();
	UE_LOG(LogTemp,Display,TEXT("Enemy Count: %d"), RemainingEnemies);

}

