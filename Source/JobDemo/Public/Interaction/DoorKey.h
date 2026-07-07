// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "DoorKey.generated.h"
class UStaticMeshComponent;

UCLASS()
class JOBDEMO_API ADoorKey : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorKey();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UStaticMeshComponent>MeshComponent;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Interact_Implementation(AActor* Interactor)override;

	FText GetInteractText_Implementation() const override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
