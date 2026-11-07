// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AISpawner.generated.h"
class UBoxComponent;
class AEnemyCharacter;
UCLASS()
class JOBDEMO_API AAISpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAISpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UBoxComponent>SpawnArea;

	UFUNCTION(BlueprintCallable,Server,Reliable)
	void SpawnOneEnemy();

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "AI")
	TSubclassOf<AEnemyCharacter>EnemyClass;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "AI")
	int SpawnNumber = -1;//如果想的话，用来指定生成具体个敌人数
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
