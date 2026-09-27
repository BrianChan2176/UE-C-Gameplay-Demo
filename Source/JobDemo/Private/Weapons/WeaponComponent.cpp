// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponComponent.h"
#include "Weapons/WeaponBase.h"
#include "Net/UnrealNetwork.h"
#include "Components/HealthComponent.h"
// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
	SetIsReplicatedByDefault(true);
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

	if (IsValid(CurrentWeapon) && IsValid(BackUpWeapon)) { return; }//判断如果武器组件里两个指针都有武器了就return

	const bool bIsEquitted=WorldWeapon->EquipTo(WeaponAttachPoint, OwnerPawn);//武器进入装备状态
	if (bIsEquitted == false) { return; }

	if (!IsValid(CurrentWeapon))
	{
		//如果当前武器槽为空，装备到当前武器槽，结束
		CurrentWeapon = WorldWeapon;
		return;
	}
	if (IsValid(CurrentWeapon) && !IsValid(BackUpWeapon))
	{
		//如果当前武器槽不为空而且备用武器槽为空，装备到备用武器槽
		BackUpWeapon = WorldWeapon;
		BackUpWeapon->SetWeaponVisibility(false);
	}
	
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWeaponComponent, CurrentWeapon);
	DOREPLIFETIME(UWeaponComponent, BackUpWeapon);
	
}

AWeaponBase* UWeaponComponent::GetWeapon() const
{
	return CurrentWeapon;
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}



void UWeaponComponent::OnRep_UpdateWeaponsMesh()
{
	if (IsValid(BackUpWeapon))
	{
		BackUpWeapon->SetWeaponVisibility(false);
	}

	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->SetWeaponVisibility(true);
	}
}

void UWeaponComponent::ServerSwitchWeapons_Implementation()
{
	if (!IsValid(CurrentWeapon) || !IsValid(BackUpWeapon)) { return; }

	AActor* OwnerActor = GetOwner();
	if (!OwnerActor) { return; }

	const UHealthComponent* Health =OwnerActor->FindComponentByClass<UHealthComponent>();

	if (!Health || Health->IsDead()) { return; }


	// 先结束旧武器正在进行的换弹
	CurrentWeapon->CancelReload();


	//交换首武器和副武器指针
	AWeaponBase* tempWeapon = CurrentWeapon;
	CurrentWeapon = BackUpWeapon;
	BackUpWeapon = tempWeapon;

	//服务器自己执行一次显示首武器mesh和隐藏副武器mesh，因为不执行OnRep
	CurrentWeapon->SetWeaponVisibility(true);
	BackUpWeapon->SetWeaponVisibility(false);
}

// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

