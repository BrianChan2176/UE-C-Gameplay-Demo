// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AmmoWidget.h"
#include "Components/TextBlock.h"
void UAmmoWidget::SetAmmoText(const FText& Text)
{
	if (AmmoText)
	{
		AmmoText->SetText(Text);
	}
}
