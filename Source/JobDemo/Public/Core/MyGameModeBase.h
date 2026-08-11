// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameModeBase.generated.h"


UCLASS()
class JOBDEMO_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMyGameModeBase();
	virtual void NotifyEnemyDied();
protected:
	virtual void BeginPlay()override;
private:
	int32 RemainingEnemies = 0;
	bool bGameFinished = false;
};
