// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponDataAsset.generated.h"

UCLASS()
class JOBDEMO_API UWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat")
    FName GunName = TEXT("Default Weapon Name");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat",meta = (ClampMin = "0.0"))
    float Damage = 25.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat",meta = (ClampMin = "0.0"))
    float FireRate = 650.f; //Round per min

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat", meta = (ClampMin = "0.0"))
    float MaxRange = 1500.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combat",meta = (ClampMin = "0.0"))
    int32 MagazineSize = 30;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo",meta = (ClampMin = "0.0"))
    float ReloadDuration = 1.5f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<UStaticMesh>GunMesh;
};
