// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponComponent.h"
#include "Weapons/WeaponBase.h"
// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// ...
}

void UWeaponComponent::SetWeaponAttachPoint(USceneComponent* AttachPoint)
{
	WeaponAttachPoint = AttachPoint;
}

void UWeaponComponent::TryPickUpWeapon(AWeaponBase* WorldWeapon)
{
	if(!IsValid(WorldWeapon)){return;}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!IsValid(WorldWeapon)) { return; }


	WorldWeapon->EquitTo(WeaponAttachPoint, OwnerPawn);
	CurrentWeapon = WorldWeapon;
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

