// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA_Fireball.h"
#include "Components/HealthComponent.h"
UGA_Fireball::UGA_Fireball()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UGA_Fireball::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	AActor* Avatar = GetAvatarActorFromActorInfo();//获得施法Actor

	UHealthComponent* Health = IsValid(Avatar) ? Avatar->FindComponentByClass<UHealthComponent>() : nullptr;
	if (!Health || Health->IsDead()) 
	{
		EndAbility(Handle,ActorInfo, ActivationInfo, true, true);
		return; 
	}

	bool bCommitSucceeded =CommitAbility(Handle, ActorInfo, ActivationInfo);//提交技能消耗和冷却

	if (!bCommitSucceeded)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UE_LOG(LogTemp, Display,TEXT(
		"Fireball activated: %s, Authority=%d"),
		*Avatar->GetName(),
		Avatar->HasAuthority());

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}


