// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/DoorKey.h"
#include "Components/MeshComponent.h"
#include "Character/MyCharacter.h"
// Sets default values
ADoorKey::ADoorKey()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

}

// Called when the game starts or when spawned
void ADoorKey::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADoorKey::Interact_Implementation(AActor* Interactor)
{
	AMyCharacter* Character = Cast<AMyCharacter>(Interactor);
	if (Character) 
	{
		Character->TookDoorKey();
		Destroy();
	}
	
}

FText ADoorKey::GetInteractText_Implementation() const
{
	return FText();
}

// Called every frame
void ADoorKey::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

