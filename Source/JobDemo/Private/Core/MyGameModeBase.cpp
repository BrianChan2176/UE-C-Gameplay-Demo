// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MyGameModeBase.h"
#include "Kismet/Gameplaystatics.h"
#include "AI/EnemyCharacter.h"
#include "Core/MyPlayerController.h"
#include "Core/MyGameStateBase.h"
#include "Engine/World.h"
#include "Core/MyGameInstance.h"
AMyGameModeBase::AMyGameModeBase()
{
	GameStateClass = AMyGameStateBase::StaticClass();
}
void AMyGameModeBase::NotifyEnemyDied()
{
	if (bGameFinished) { return; }

	RemainingEnemies = RemainingEnemies - 1;
	UE_LOG(LogTemp, Display, TEXT("Enemy Count: %d"), RemainingEnemies);

	// 还有敌人，继续战斗
	if (RemainingEnemies > 0) { return; }

	bGameFinished = true;//结束游戏

	// 获取当前地图名，true 表示去掉编辑器运行时添加的前缀
	const FString CurrentLevelName =UGameplayStatics::GetCurrentLevelName(this, true);

	UMyGameInstance* GI =Cast<UMyGameInstance>(GetGameInstance());


	// 第一关清空：进入第二关
	if (CurrentLevelName == TEXT("NewWorld"))
	{
		GetWorld()->ServerTravel(TEXT("/Game/Maps/LevelTwo"));
		GI->ApplyDamageUpdate();
		return;
	}

	// 第二关清空：进入第三关
	if (CurrentLevelName == TEXT("LevelTwo"))
	{
		GetWorld()->ServerTravel(TEXT("/Game/Maps/LevelThree"));
		GI->ApplyDamageUpdate();
		return;
	}

	// 第三关清空：显示最终胜利
	if (CurrentLevelName == TEXT("LevelThree"))
	{
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

	FTimerHandle TimerHandle;

	GetWorldTimerManager().SetTimer(
		TimerHandle,
		[this]()
		{
			TArray<AActor*> Enemies;
			UGameplayStatics::GetAllActorsOfClass(
				this,
				AEnemyCharacter::StaticClass(),
				Enemies
			);

			RemainingEnemies = Enemies.Num();

			UE_LOG(LogTemp, Display, TEXT("Enemy Count: %d"), RemainingEnemies);
		},
		1.f,	
		false
	);


/*	TArray<AActor*>Enemies;
	UGameplayStatics::GetAllActorsOfClass(this, AEnemyCharacter::StaticClass(), Enemies);
	RemainingEnemies = Enemies.Num();
	UE_LOG(LogTemp,Display,TEXT("Enemy Count: %d"), RemainingEnemies);*/

}

