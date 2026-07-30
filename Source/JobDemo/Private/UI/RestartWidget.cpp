// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RestartWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
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
	const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this, true);
	if (CurrentLevelName.IsEmpty()) { return; }
	UGameplayStatics::OpenLevel(this, FName(*CurrentLevelName));

}
