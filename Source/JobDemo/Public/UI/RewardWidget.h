// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "RewardWidget.generated.h"

class UButton;
UCLASS()
class JOBDEMO_API URewardWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UButton* UpgradeSpeedButton;

	UPROPERTY(meta = (BindWidget))
	UButton* UpgradeDamageButton;

	UPROPERTY(meta = (BindWidget))
	UButton* UpgradeJumpButton;


};
