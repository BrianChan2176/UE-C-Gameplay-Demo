// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponBase.h"
#include "Weapons/WeaponDataAsset.h"
// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (!WeaponData)
	{
		UE_LOG(LogTemp, Warning, TEXT("没有武器数据配置"));
		return;
	}

	ApplyWeaponData();
}

void AWeaponBase::ApplyWeaponData()
{
	if (!WeaponData)
	{
		UE_LOG(LogTemp, Warning, TEXT("没有武器数据配置"));
		return;
	}

	CurrentAmmo = WeaponData->MagazineSize;

	UE_LOG(LogTemp, Display, TEXT("武器数据初始化成功"));
}

bool AWeaponBase::CanShoot() const
{
	if (CurrentAmmo > 0 && bIsReloading == false)
	{
		return true;
	}
	return false;
}

void AWeaponBase::Fire()
{
	if (!CanShoot())
	{
		UE_LOG(LogTemp, Warning, TEXT("武器当前不能射击"));
		return;
	}

	CurrentAmmo = CurrentAmmo - 1;

	UE_LOG(LogTemp, Warning, TEXT("武器射击成功，剩下弹药：%d"),CurrentAmmo);
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

