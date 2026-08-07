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

		//服务器GameMODE用World里的PlayerController迭代器遍历所有PlayerController调用Client RPC命令，每个客户端显示胜利UI
		FConstPlayerControllerIterator PlayerControllerIT = GetWorld()->GetPlayerControllerIterator();
		for (PlayerControllerIT;PlayerControllerIT;++PlayerControllerIT)
		{
			AMyPlayerController* PlayerController = Cast<AMyPlayerController>(PlayerControllerIT->Get());
			if (PlayerController) 
			{ 
				PlayerController->ClientVictory(); 
			}
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

