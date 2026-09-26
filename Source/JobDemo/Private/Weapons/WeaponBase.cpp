// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponBase.h"
#include "Weapons/WeaponDataAsset.h"
#include "Components/StaticMeshComponent.h"
#include "Character/MyCharacter.h"
#include "Weapons/WeaponComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "Components/HealthComponent.h"
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
	if (HasAuthority())
	{
		CurrentAmmo = WeaponData->MagazineSize;
	}
	StaticMeshComponent->SetStaticMesh(WeaponData->GunMesh);
	UE_LOG(LogTemp, Display, TEXT("%s武器数据初始化成功"), *WeaponData->GunName.ToString());
}

bool AWeaponBase::CanShoot() const
{
	if (CurrentAmmo <= 0)
	{
		return false;
	}

	//射速限制的判断条件：现在的世界时间-上一发开火的世界时间>=射击CD 才允许射击。返回false不能shoot现在判断式写相反
	if (!WeaponData) { return false; }
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (PreviousShotTime >=0 && CurrentTime - PreviousShotTime< WeaponData->ShootCD) { return false; }//如果已经开过首枪而且距离上次射击时间小于CD不可以射击

	//判断正在换弹不能射击
	if (bReloading) { return false; }

	return true;
}

bool AWeaponBase::Fire()
{
	if (!WeaponData)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s没有武器数据配置"), *GetName());
		return false;
	}

	if (!CanShoot())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s当前不能射击"),*WeaponData->GunName.ToString());
		return false;
	}

	PreviousShotTime= GetWorld()->GetTimeSeconds();//记录上一发子弹的世界时间
	CurrentAmmo = CurrentAmmo - 1;//扣枪里的子弹

	UE_LOG(LogTemp, Warning, TEXT("%s射击成功，剩下弹药：%d，本次开火世界时间：%f"), *WeaponData->GunName.ToString(),CurrentAmmo,PreviousShotTime);
	return true;
}

const UWeaponDataAsset* AWeaponBase::GetWeaponData() const
{
	return WeaponData;
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
	bIsEquipped = true;
	return true;
}



void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeaponBase, bIsEquipped);
	DOREPLIFETIME(AWeaponBase, CurrentAmmo);
	DOREPLIFETIME(AWeaponBase, bReloading);
}

void AWeaponBase::OnRep_Equipped()
{
	StaticMeshComponent->SetSimulatePhysics(false);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeaponBase::Reload_Implementation()
{
	if (!WeaponData) { return; }
	if (bReloading) { return; }
	if (CurrentAmmo == WeaponData->MagazineSize) { return; }
	if (!GetWorld()) { return; }

	//如果死亡不能换弹
	AActor* WeaponOwner = GetOwner();
	if (!WeaponOwner) { return; }
	const UHealthComponent* Health =WeaponOwner->FindComponentByClass<UHealthComponent>();
	if (!Health || Health->IsDead()) { return; }

	bReloading = true;
	UE_LOG(LogTemp, Display, TEXT("Reloading..."));
	//假设播放换弹动画，暂无
	GetWorld()->GetTimerManager().SetTimer(
		ReloadTimer,
		this,
		&AWeaponBase::ReloadCompleted,
		WeaponData->ReloadDuration,
		false
	);
	
}

void AWeaponBase::ReloadCompleted()
{
	if (!WeaponData) { return; }
	CurrentAmmo = WeaponData->MagazineSize;
	bReloading = false;
	UE_LOG(LogTemp, Display, TEXT("Reloaded"));
}

void AWeaponBase::CancelReload_Implementation()
{
	if (!GetWorld()) { return; }
	GetWorldTimerManager().ClearTimer(ReloadTimer);
	bReloading = false;
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

