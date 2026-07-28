// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerHealthWidget.h"
#include "Components/ProgressBar.h"
void UPlayerHealthWidget::SetHealth(float CurrentHealth, float MaxHealth)
{
	if (!PlayerHealthProgressBar){return;}
	if (MaxHealth <= 0) { return; }
	PlayerHealthProgressBar->SetPercent(FMath::Clamp((CurrentHealth / MaxHealth), 0.f, 1.f));

}
