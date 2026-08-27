// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponBase.h"
#include "Weapons/WeaponDataAsset.h"
#include "Components/StaticMeshComponent.h"
#include "Character/MyCharacter.h"
#include "Weapons/WeaponComponent.h"
// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	SetRootComponent(StaticMeshComponent);

	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	StaticMeshComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (!WeaponData)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s没有武器数据配置"), *GetName());
		return;
	}

	ApplyWeaponData();

}

void AWeaponBase::ApplyWeaponData()
{
	if (!WeaponData)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s没有武器数据配置"), *GetName());
		return;
	}

	CurrentAmmo = WeaponData->MagazineSize;
	StaticMeshComponent->SetStaticMesh(WeaponData->GunMesh);
	UE_LOG(LogTemp, Display, TEXT("%s武器数据初始化成功"), *WeaponData->GunName.ToString());
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
	if (!WeaponData)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s没有武器数据配置"), *GetName());
		return;
	}

	if (!CanShoot())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s当前不能射击"),*WeaponData->GunName.ToString());
		return;
	}

	CurrentAmmo = CurrentAmmo - 1;

	UE_LOG(LogTemp, Warning, TEXT("%s射击成功，剩下弹药：%d"), *WeaponData->GunName.ToString(),CurrentAmmo);
}

void AWeaponBase::Interact_Implementation(AActor* Interactor)
{
	if (!Interactor || !WeaponData) { return; }
	UE_LOG(LogTemp, Warning, TEXT("%s可交互"), *WeaponData->GunName.ToString(), CurrentAmmo);
	//只关心判断有没有武器组件，不需要关心是不是Character所以不用Cast和降低没有必要的耦合
	UWeaponComponent* PlayerWeaponComponent= Interactor->FindComponentByClass<UWeaponComponent>();
	if (!IsValid(PlayerWeaponComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("没有武器组件"));
		return;
	}

	PlayerWeaponComponent->TryPickUpWeapon(this);
}

bool AWeaponBase::EquipTo(USceneComponent* AttachPoint, APawn* OwnerPawn)//进入装备状态
{
	if (!IsValid(AttachPoint) || !IsValid(OwnerPawn) || !IsValid(StaticMeshComponent) || !HasAuthority() || IsValid(GetOwner()))
	{
		return false;
	}

	StaticMeshComponent->SetSimulatePhysics(false);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SetOwner(OwnerPawn);
	const bool bIsAttached=AttachToComponent(AttachPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	SetInstigator(OwnerPawn);

	if (bIsAttached == false) 
	{
		SetOwner(nullptr);
		SetInstigator(nullptr);
		StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		return false;
	}

	return true;
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

