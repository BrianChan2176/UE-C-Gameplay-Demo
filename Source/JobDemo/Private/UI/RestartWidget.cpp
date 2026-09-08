// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RestartWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Core/MyPlayerController.h"
void URestartWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RestartButton) 
	{
		RestartButton->OnClicked.AddDynamic(this, &URestartWidget::RestartButtonClicked);
	}
}

void URestartWidget::NativeDestruct()
{
	if (RestartButton)
	{
		RestartButton->OnClicked.RemoveDynamic(this, &URestartWidget::RestartButtonClicked);
	}
	Super::NativeDestruct();
}

void URestartWidget::RestartButtonClicked()
{
	AMyPlayerController* PC =
		Cast<AMyPlayerController>(GetOwningPlayer());

	if (PC)
	{
		PC->ServerRestart();
	}
}
