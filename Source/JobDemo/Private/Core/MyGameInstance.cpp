// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MyGameInstance.h"

void UMyGameInstance::ApplyDamageUpdate()
{
	DamageMultiplier = DamageMultiplier + 1.2;
}

void UMyGameInstance::ResetRun()
{
    DamageMultiplier = 1.f;

    UE_LOG(LogTemp, Display, TEXT("本局成长已重置"));
}
