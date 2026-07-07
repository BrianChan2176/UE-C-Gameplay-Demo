// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractPromptWidget.generated.h"

class UTextBlock;
UCLASS()
class JOBDEMO_API UInteractPromptWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetPromptText(const FText& Text);
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock>PromptText;
};
