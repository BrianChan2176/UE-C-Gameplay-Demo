// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Door.generated.h"
class UStaticMeshComponent;
class USceneComponent;
UCLASS()
class JOBDEMO_API ADoor : public AActor,public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

	void Interact_Implementation(AActor* Interactor)override;

	FText GetInteractText_Implementation()const override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DoorComponent")
	TObjectPtr<UStaticMeshComponent>DoorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "DoorComponent")
	TObjectPtr<USceneComponent>Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DoorComponent",Replicated)
	TObjectPtr<USceneComponent>PivotPoint;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "DoorComponent",Replicated)
	bool bIsOpen = false;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:	


};
