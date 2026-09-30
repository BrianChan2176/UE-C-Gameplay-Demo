// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponDataAsset.generated.h"

class USoundBase;
class UNiagaraSystem;
class UAnimMontage;
UCLASS()
class JOBDEMO_API UWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat")
    FName GunName = TEXT("Default Weapon Name");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat",meta = (ClampMin = "0.0"))
    float Damage = 25.f;


    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat", meta = (ClampMin = "0.0"))
    float MaxRange = 1500.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat",meta = (ClampMin = "0.0"))
    int32 MagazineSize = 30;


    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<UStaticMesh>GunMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat", meta = (ClampMin = "0.0"))
    float ShootCD = 0.4f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat", meta = (ClampMin = "0.0"))
    float ReloadDuration = 1.5f;


    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat")
    TObjectPtr<USoundBase>FireSound=nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Effects")
    TObjectPtr<UNiagaraSystem>MuzzleFlash = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Effects")
    FName MuzzleSocketName = TEXT("Muzzle");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animation")
    TObjectPtr<UAnimMontage>ReloadMontage = nullptr;


    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat")
    float VerticalRecoilStrength = 1.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat")
    float HorizontalRecoilStrength = 1.f;
};
