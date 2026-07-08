// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnemyAIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "AI/EnemyCharacter.h"

AEnemyAIController::AEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemyAIController::OnPossess(APawn* ControlledPawn)
{
	Super::OnPossess(ControlledPawn);

	CurrentPatrolIndex = 0;
	MoveToCurrentPatrolPoint();

/*	GetWorldTimerManager().SetTimer(
		MoveToRandomLocationTimer,
		this,
		&AEnemyAIController::MoveToRandomLocation,
		FMath::FRandRange(1.f, 2.f),
		true);*/

}

void AEnemyAIController::MoveToCurrentPatrolPoint()
{
	AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(GetPawn());
	if (!EnemyCharacter) { return; }

	if (EnemyCharacter->PatrolPointsArray.Num() == 0) { return; }

	if (!EnemyCharacter->PatrolPointsArray.IsValidIndex(CurrentPatrolIndex)) { CurrentPatrolIndex = 0; }

	AActor* PatrolPoint = EnemyCharacter->PatrolPointsArray[CurrentPatrolIndex];
	if (!PatrolPoint)
	{
		GoToNextPatrolPoint();
		return;
	}

	MoveToActor(PatrolPoint, AcceptanceRadius);
}


void AEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (Result.Code==EPathFollowingResult::Success) 
	{
		GetWorldTimerManager().SetTimer(WaitForNextPatrolTimer, this, &AEnemyAIController::GoToNextPatrolPoint, PatrolWaitTime, false);
	}
	else
	{
		GetWorldTimerManager().SetTimer(WaitForNextPatrolTimer,
			this,
			&AEnemyAIController::GoToNextPatrolPoint,
			0.2f,
			false);//要有Timer延迟一下不然会:GoToNextPatrolPoint→MoveToCurrentPatrolPoint→回来OnMoveCompleted，又失败得太快然后递归太深最后StackOverFlow

		return;
	}
}

void AEnemyAIController::GoToNextPatrolPoint()
{
	AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(GetPawn());
	if (!EnemyCharacter) { return; }

	const int32 PatrolPointsCount=EnemyCharacter->PatrolPointsArray.Num();
	if (PatrolPointsCount == 0) {return;}

	CurrentPatrolIndex = (CurrentPatrolIndex + 1) % PatrolPointsCount;
	MoveToCurrentPatrolPoint();
}


/*void AEnemyAIController::MoveToRandomLocation()
{
	if (GetMoveStatus() == EPathFollowingStatus::Moving){return;}


	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) { return; }

	UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavigationSystem) { return; }

	FNavLocation RandomLocation;
	const bool bFoundLocation = NavigationSystem->GetRandomReachablePointInRadius(
		ControlledPawn->GetActorLocation(),
		PatrolRadius,
		RandomLocation
	);

	if (bFoundLocation)
	{
		MoveToLocation(RandomLocation.Location, EnoughRadius);
	}
	
}*/

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

/*	if (GetMoveStatus() != EPathFollowingStatus::Moving) 
	{
		MoveToRandomLocation();
	}*/
}