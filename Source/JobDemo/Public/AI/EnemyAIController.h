// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;

enum class EEnemyState : uint8
{
	Patrol,
	Chasing
};

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
	float AcceptanceRadius = 0.f;

	FTimerHandle StartPatrolTimerHandle;
	
	int32 CurrentPatrolIndex=0;
	void MoveToCurrentPatrolPoint();
	void GoToNextPatrolPoint();
	FTimerHandle WaitForNextPatrolTimer;
	float PatrolWaitTime = 2.f;

	//感知玩家
	UPROPERTY(VisibleAnywhere, Category = "AI Perception")
	TObjectPtr<UAIPerceptionComponent>AIPerceptionComponent;
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Sight>SightConfig;

	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* Target,FAIStimulus Stimulus);

	//追击玩家
	EEnemyState CurrentState = EEnemyState::Patrol;//AI当前状态
	UPROPERTY()
	TObjectPtr<AActor>TargetPlayer=nullptr;//当前追击的玩家
	UPROPERTY(EditDefaultsOnly, Category = "AI Chase")
	float ChaseAcceptanceDistance = 100.f;//追逐玩家足够距离停下

	void StartChasing(AActor* Target);
	void StopChasingAndResumePatrol();
};
