// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class JOBDEMO_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Run")
	float DamageMultiplier = 1.f;
	UFUNCTION(BlueprintCallable, Category = "Run")
	void ApplyDamageUpdate();

	UFUNCTION(BlueprintCallable, Category = "Run")
	void ResetRun();
};
