// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnemyCharacter.h"
#include "AI/EnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Core/MyGameModeBase.h"
#include "Engine/World.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 300.0f, 0.0f);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	//支持网络复制
	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (HealthComponent) 
	{
		HealthComponent->OnDeath.AddDynamic(this, &AEnemyCharacter::HandleDeath);
	}

	
}

void AEnemyCharacter::HandleDeath()
{
	AEnemyAIController* EnemyController = Cast<AEnemyAIController>(GetController());
	if (EnemyController)
	{
		EnemyController->StopMovement();
		EnemyController->ClearFocus(EAIFocusPriority::Gameplay);
		EnemyController->UnPossess();
		EnemyController->Destroy();
	}

	GetCharacterMovement()->DisableMovement();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	USkeletalMeshComponent* MeshComponent = GetMesh();
	if (MeshComponent && MeshComponent->GetPhysicsAsset())
	{
		MeshComponent->SetCollisionProfileName(TEXT("Ragdoll"));//给MeshComponent使用名为 Ragdoll 的碰撞预设规则
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//开启两类碰撞：Query允许射线检测 Physics允许真实物理碰撞
		MeshComponent->SetSimulatePhysics(true);//让身体骨骼网格开始进行物理模拟。
		MeshComponent->WakeAllRigidBodies();//让所有骨骼刚体立即活动
	}
	else { UE_LOG(LogTemp, Display, TEXT("%s Has no SkeletalMesh Asset"), *GetName()); }

	float DestroyDelay = 3.f;
	//通知GameMode有敌人死亡
	if (HasAuthority())
	{
		AMyGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AMyGameModeBase>();
		if (GameMode)
		{
			GameMode->NotifyEnemyDied();
		}
		SetLifeSpan(DestroyDelay);
	}
}

void AEnemyCharacter::AttackTarget(AActor* Target)
{
	if (!Target) { return; }

	AController* OwnerController = GetController();
	if (!OwnerController) { return; }

	FVector ViewLocation;
	FRotator ViewRotator;
	OwnerController->GetPlayerViewPoint(ViewLocation, ViewRotator);

	FVector Start = ViewLocation;

	FVector ForwardDirection = ViewRotator.Vector();
	FVector End = Start + ForwardDirection * AIAttackRange;

	FHitResult HitResult;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	//旋转敌人身体面向玩家
	FVector PlayerDirection = Target->GetActorLocation() - GetActorLocation();
	SetActorRotation(PlayerDirection.Rotation());

	//继续射线
	bool bHitted = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
	#if !UE_BUILD_SHIPPING
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.f, 0, 2.f);
	#endif
	if (bHitted)
	{
		#if !UE_BUILD_SHIPPING
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 12, FColor::Green, false, 2.0f);
		#endif
		AActor* HittedActor = HitResult.GetActor();
		if (!HittedActor) { return; }
		UGameplayStatics::ApplyDamage(HittedActor, AIDamage, OwnerController, this, UDamageType::StaticClass());
		UE_LOG(LogTemp, Display, TEXT("AI 攻击了 %s"), *HittedActor->GetName());
	}
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

