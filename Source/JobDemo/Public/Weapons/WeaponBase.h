// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/Interactable.h"
#include "WeaponBase.generated.h"
class UWeaponDataAsset;
class UStaticMeshComponent;
UCLASS()
class JOBDEMO_API AWeaponBase : public AActor,public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

	UFUNCTION(BlueprintPure, Category = "Weapon")
	bool CanShoot()const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Fire();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Data")
	TObjectPtr<UWeaponDataAsset>WeaponData;//武器用哪个数据配置凹槽

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon Data")
	int32 CurrentAmmo=0;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void ApplyWeaponData();//把数据从数据配置拿到实例

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon Data")
	bool bIsReloading = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Data")
	TObjectPtr<UStaticMeshComponent>StaticMeshComponent;


	virtual void Interact_Implementation(AActor* Interactor)override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
