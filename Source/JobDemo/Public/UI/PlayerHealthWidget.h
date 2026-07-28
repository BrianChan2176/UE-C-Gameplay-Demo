// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHealthWidget.generated.h"
class UProgressBar;
/**
 * 
 */
UCLASS()
class JOBDEMO_API UPlayerHealthWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetHealth(float CurrentHealth, float MaxHealth);
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar>PlayerHealthProgressBar;
};
