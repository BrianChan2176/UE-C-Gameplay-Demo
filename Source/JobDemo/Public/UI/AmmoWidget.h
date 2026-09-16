// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoWidget.generated.h"


class UTextBlock;
UCLASS()
class JOBDEMO_API UAmmoWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetAmmoText(const FText& Text);
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock>AmmoText;
};
