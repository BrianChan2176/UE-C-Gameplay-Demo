// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AISpawner.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AI/EnemyCharacter.h"
// Sets default values
AAISpawner::AAISpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
    SpawnArea->SetIsReplicated(true);
	RootComponent = SpawnArea;
	SpawnArea->SetBoxExtent(FVector(600.f, 300.f, 50.f));
	SpawnArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AAISpawner::BeginPlay()
{
	Super::BeginPlay();
    int32 RandomSpawnNumber = FMath::RandRange(3, 6);//随机敌人数量
    for (int i=0;i<RandomSpawnNumber;i++)
    {
        SpawnOneEnemy();
    }
}

void AAISpawner::SpawnOneEnemy_Implementation()
{
    auto BoxExtent = SpawnArea->GetScaledBoxExtent();

    float RandomX = UKismetMathLibrary::RandomFloatInRange(-BoxExtent.X, BoxExtent.X);
    float RandomY = UKismetMathLibrary::RandomFloatInRange(-BoxExtent.Y, BoxExtent.Y);

    auto LocalPos = UKismetMathLibrary::MakeVector(RandomX, RandomY, 0);

    auto WorldPos = UKismetMathLibrary::TransformLocation(SpawnArea->GetComponentTransform(), LocalPos);
    auto End = UKismetMathLibrary::Subtract_VectorVector(WorldPos, FVector(0, 0, 60000));

    TArray<AActor*> Ignore;
    FHitResult HitResult;
    UKismetSystemLibrary::LineTraceSingle(
        this,
        WorldPos,
        End,
        ETraceTypeQuery::TraceTypeQuery1,
        false,
        Ignore,
        EDrawDebugTrace::ForDuration,
        HitResult,
        true
    );

    auto AILocation = UKismetMathLibrary::Add_VectorVector(
        HitResult.Location,
        FVector(0, 0, 87)
    );

    FTransform SpawnTransform(AILocation);

    GetWorld()->SpawnActor<AEnemyCharacter>(EnemyClass, SpawnTransform);
}

// Called every frame
void AAISpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

