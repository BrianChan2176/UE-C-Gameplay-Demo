// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class JOBDEMO_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
public:
	AEnemyAIController();

protected:
	virtual void OnPossess(APawn* ControlledPawn) override;

	virtual void Tick(float DeltaTime)override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float PatrolRadius = 1700.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float EnoughRadius = 80.f;

	void MoveToRandomLocation();

	FTimerHandle MoveToRandomLocationTimer;
};
