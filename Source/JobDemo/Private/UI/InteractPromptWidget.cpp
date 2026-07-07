// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InteractPromptWidget.h"
#include "Components/TextBlock.h"
void UInteractPromptWidget::SetPromptText(const FText& Text)
{
	if (PromptText)
	{
		PromptText->SetText(Text);
	}
}
