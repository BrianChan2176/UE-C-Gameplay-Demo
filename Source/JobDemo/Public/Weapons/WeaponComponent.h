// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

class AWeaponBase;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class JOBDEMO_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponComponent();

	void SetWeaponAttachPoint(USceneComponent* AttachPoint);

	UFUNCTION()
	void TryPickUpWeapon(AWeaponBase* WorldWeapon);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure)
	AWeaponBase* GetWeapon()const;

	UFUNCTION(Server, Reliable)
	void ServerSwitchWeapons();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon",ReplicatedUsing= OnRep_UpdateWeaponsMesh)
	TObjectPtr<AWeaponBase>CurrentWeapon;//当前使用武器Actor

	UPROPERTY()
	TObjectPtr<USceneComponent>WeaponAttachPoint;//引用角色武器挂点


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", ReplicatedUsing = OnRep_UpdateWeaponsMesh)
	TObjectPtr<AWeaponBase>BackUpWeapon;//后备武器Actor

	UFUNCTION()
	void OnRep_UpdateWeaponsMesh();


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
