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
	if (!IsValid(OwnerPawn)) { return; }

	if (IsValid(CurrentWeapon)) { return; }//判断如果武器组件上已经有武器就装备不了新的世界武器

	const bool bIsEquitted=WorldWeapon->EquipTo(WeaponAttachPoint, OwnerPawn);//武器进入装备状态
	if (bIsEquitted == false) { return; }

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

