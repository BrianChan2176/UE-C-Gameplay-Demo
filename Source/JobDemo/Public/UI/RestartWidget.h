// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RestartWidget.generated.h"

class UButton;

UCLASS()
class JOBDEMO_API URestartWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct()override;
	virtual void NativeDestruct()override;
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton>RestartButton;

	UFUNCTION()
	void RestartButtonClicked();
};
