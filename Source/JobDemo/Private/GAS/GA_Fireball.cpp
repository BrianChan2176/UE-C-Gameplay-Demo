// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA_Fireball.h"
#include "Components/HealthComponent.h"
#include "GAS/FireballProjectile.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"

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
	APawn* CasterPawn =Cast<APawn>(GetAvatarActorFromActorInfo()); //获得施法Actor

	UWorld* AbilityWorld = GetWorld();
	if (!IsValid(CasterPawn) ||
		!CasterPawn->HasAuthority() ||
		!AbilityWorld ||
		!FireballProjectileClass)
	{
		UE_LOG(LogTemp, Warning,TEXT("Fireball: check caster and projectile class"));

		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UHealthComponent* Health = IsValid(CasterPawn) ? CasterPawn->FindComponentByClass<UHealthComponent>() : nullptr;
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

	// 从角色中心向上 50 厘米，作为本轮的发射位置
	const FVector SpawnLocation =CasterPawn->GetActorLocation() + FVector(0.f, 0.f, 50.f);

	// 使用服务器掌握的瞄准朝向
	const FRotator SpawnRotation = CasterPawn->GetBaseAimRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = CasterPawn;
	SpawnParams.Instigator = CasterPawn;

	// 允许从施法者身体范围内生成，弹体移动时会忽略施法者
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AFireballProjectile* SpawnedFireball =
		AbilityWorld->SpawnActor<AFireballProjectile>(
			FireballProjectileClass,
			SpawnLocation,
			SpawnRotation,
			SpawnParams);

	if (!IsValid(SpawnedFireball))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}


	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}


