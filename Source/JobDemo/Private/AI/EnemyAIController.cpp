// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnemyAIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "AI/EnemyCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/GameplayStatics.h"

AEnemyAIController::AEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;//要开因为AI Perception靠Controller的ControllRotation更新，关了Controller不更新→Perception不更新→视觉锥会固定

	//设置感知组件
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SetPerceptionComponent(*AIPerceptionComponent);
	//设置视觉配置
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1200.f;
	SightConfig->LoseSightRadius = 1500.f;
	SightConfig->PeripheralVisionAngleDegrees = 60.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	// 把视觉配置交给感知组件
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	//感知组件订阅感知状态变化广播
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::HandleTargetPerceptionUpdated);

	
}

void AEnemyAIController::OnPossess(APawn* ControlledPawn)
{
	Super::OnPossess(ControlledPawn);

	CurrentPatrolIndex = 0;

	GetWorldTimerManager().SetTimer(//第一次移动要等东西加载，不然会失败直接跳过第一个目标点
		StartPatrolTimerHandle,
		this,
		&AEnemyAIController::MoveToCurrentPatrolPoint,
		1.f,
		false);

}

void AEnemyAIController::MoveToCurrentPatrolPoint()
{
	if (CurrentState != EEnemyState::Patrol) { return; }

	AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(GetPawn());
	if (!EnemyCharacter) { return; }

	if (EnemyCharacter->PatrolPointsArray.Num() == 0) { return; }

	if (!EnemyCharacter->PatrolPointsArray.IsValidIndex(CurrentPatrolIndex)) { CurrentPatrolIndex = 0; }

	AActor* PatrolPoint = EnemyCharacter->PatrolPointsArray[CurrentPatrolIndex];
	if (!PatrolPoint)
	{
		GetWorldTimerManager().SetTimer(
			WaitForNextPatrolTimer,
			this,
			&AEnemyAIController::GoToNextPatrolPoint,
			0.2f,
			false
		);
		return;
	}

	const EPathFollowingRequestResult::Type MoveResult =MoveToActor(PatrolPoint, AcceptanceRadius);

	if (MoveResult == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		GetWorldTimerManager().SetTimer(
			WaitForNextPatrolTimer,
			this,
			&AEnemyAIController::GoToNextPatrolPoint,
			PatrolWaitTime,
			false
		);
	}
	else if (MoveResult == EPathFollowingRequestResult::Failed)
	{
		UE_LOG(LogTemp, Warning, TEXT("MoveTo PatrolPoint failed: %s"), *GetNameSafe(PatrolPoint));

		GetWorldTimerManager().SetTimer(
			WaitForNextPatrolTimer,
			this,
			&AEnemyAIController::GoToNextPatrolPoint,
			PatrolWaitTime,
			false
		);
	}
}


void AEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	if (CurrentState == EEnemyState::Chasing) { return; }

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
			false);//要有Timer延迟一下不然失败的时候会:GoToNextPatrolPoint→MoveToCurrentPatrolPoint→回来OnMoveCompleted，又失败得太快然后递归太深最后StackOverFlow

		return;
	}
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

}

void AEnemyAIController::GoToNextPatrolPoint()
{
	if (CurrentState != EEnemyState::Patrol) { return; }
	AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(GetPawn());
	if (!EnemyCharacter) { return; }

	const int32 PatrolPointsCount=EnemyCharacter->PatrolPointsArray.Num();
	if (PatrolPointsCount == 0) {return;}

	CurrentPatrolIndex = (CurrentPatrolIndex + 1) % PatrolPointsCount;
	MoveToCurrentPatrolPoint();
}

void AEnemyAIController::HandleTargetPerceptionUpdated(AActor* Target, FAIStimulus Stimulus)
{
	APawn* PlayerPawn=UGameplayStatics::GetPlayerPawn(this, 0);
	if (Target != PlayerPawn)
	{
		return;
	}
	if (Stimulus.WasSuccessfullySensed()) 
	{
		UE_LOG(LogTemp, Display, TEXT("AI成功感知到玩家"));

		StartChasing(Target);

		GetWorldTimerManager().SetTimer(ChaseUpdateTimerHandle,
			this,
			&AEnemyAIController::UpdateChase,
			ChaseUpdateInveral,
			true
		);
	}
	else 
	{
		UE_LOG(LogTemp, Display, TEXT("AI失去感知玩家")); 
		GetWorldTimerManager().ClearTimer(ChaseUpdateTimerHandle);
		StopChasingAndResumePatrol();
	}
}

void AEnemyAIController::StartChasing(AActor* Target)
{
	if (!Target) { return; }
	//一定要先切换状态，不然停止巡逻可能会触发OnMoveCompeleted
	CurrentState = EEnemyState::Chasing;
	TargetPlayer = Target;
	// 防止等待巡逻的Timer突然触发
	GetWorldTimerManager().ClearTimer(WaitForNextPatrolTimer);
	// 停止当前巡逻移动
	StopMovement();
	//面向Target玩家
	SetFocus(Target);
	//追击玩家
	MoveToActor(Target, ChaseAcceptanceDistance);
	UE_LOG(LogTemp, Display, TEXT("AI开始追击玩家"));
}

void AEnemyAIController::StopChasingAndResumePatrol()
{
	if (CurrentState != EEnemyState::Chasing) { return; }
	//停止追击，清除玩家目标
	StopMovement();
	ClearFocus(EAIFocusPriority::Gameplay);
	CurrentState = EEnemyState::Patrol;
	TargetPlayer = nullptr;
	//恢复巡逻
	MoveToCurrentPatrolPoint();
	UE_LOG(LogTemp, Display, TEXT("AI停止追击玩家，返回巡逻"));
}

void AEnemyAIController::UpdateChase()
{
	//移动追击玩家
	if (!TargetPlayer) { return; }
	if (CurrentState != EEnemyState::Chasing) { return; }


	//用攻击距离区分追击和攻击状态
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) { return; }
	const float TargetDistance = FVector::Dist(ControlledPawn->GetActorLocation(), TargetPlayer->GetActorLocation());
	if (TargetDistance > AIAttackRange) //如果没进入攻击距离就只追击玩家
	{
		if (GetMoveStatus() != EPathFollowingStatus::Moving) 
		{
			MoveToActor(TargetPlayer, ChaseAcceptanceDistance);
		}
		return;
	}
	 
	//已经进入攻击距离就停止移动并攻击玩家
	StopMovement();
	if (!bCanAttack) { return; }

	AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(GetCharacter());
	if (EnemyCharacter && TargetPlayer)
	{
		EnemyCharacter->AttackTarget(TargetPlayer);
		bCanAttack = false;
		GetWorldTimerManager().SetTimer(AttackCoolDownTimerHandle, this, &AEnemyAIController::ResetAttackCoolDown, AttackCoolDown, false);
	}

}

void AEnemyAIController::ResetAttackCoolDown()
{
	bCanAttack = true;

}



void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}  