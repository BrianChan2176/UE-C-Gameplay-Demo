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

	virtual void OnMoveCompleted(FAIRequestID RequestID,const FPathFollowingResult& Result)override;

	
private:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float PatrolRadius = 1700.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float AcceptanceRadius = 80.f;

	/*void MoveToRandomLocation();
	FTimerHandle MoveToRandomLocationTimer;*/

	
	int32 CurrentPatrolIndex=0;
	void MoveToCurrentPatrolPoint();
	void GoToNextPatrolPoint();
	FTimerHandle WaitForNextPatrolTimer;
	float PatrolWaitTime = 3.f;
};
