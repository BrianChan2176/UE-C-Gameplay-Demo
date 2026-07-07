// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Door.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	PivotPoint = CreateDefaultSubobject<USceneComponent>(TEXT("PivotPoint"));
	PivotPoint->SetupAttachment(Root);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(PivotPoint);
}

void ADoor::Interact_Implementation(AActor* Interactor)
{
	if (bIsOpen == false) 
	{
		PivotPoint->AddWorldRotation(FRotator(0.f, 90.f, 0.f));
		bIsOpen = true;
	}
	else { PivotPoint->AddWorldRotation(FRotator(0.f, -90.f, 0.f)); 		bIsOpen = false;}
	
}

FText ADoor::GetInteractText_Implementation()
{
	return FText::FromString(TEXT("按 F 开门"));
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

