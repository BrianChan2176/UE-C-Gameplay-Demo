// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MyGameStateBase.h"
#include "Net/UnrealNetwork.h"
void AMyGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyGameStateBase, bTeamHasDoorKey);
}

void AMyGameStateBase::TeamHasDoorKey()
{
	if (!HasAuthority()) { return; }
	bTeamHasDoorKey = true;
}
