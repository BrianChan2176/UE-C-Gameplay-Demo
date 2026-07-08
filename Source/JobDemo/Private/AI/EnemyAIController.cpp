// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnemyAIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

AEnemyAIController::AEnemyAIController()
{
}

void AEnemyAIController::OnPossess(APawn* ControlledPawn)
{
	Super::OnPossess(ControlledPawn);

	GetWorldTimerManager().SetTimer(
		MoveToRandomLocationTimer,
		this,
		&AEnemyAIController::MoveToRandomLocation,
		FMath::FRandRange(1.f, 2.f),
		true);

}


void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

/*	if (GetMoveStatus() != EPathFollowingStatus::Moving) 
	{
		MoveToRandomLocation();
	}*/
}


void AEnemyAIController::MoveToRandomLocation()
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
	
}
