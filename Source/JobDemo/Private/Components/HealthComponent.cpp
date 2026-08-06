// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HealthComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner) { return; }

	if (Owner->HasAuthority()) 
	{
		CurrentHealth = MaxHealth;
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
	}
	
}


void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, CurrentHealth);
	DOREPLIFETIME(UHealthComponent, bIsDead);
}

void UHealthComponent::OnRep_Dead()
{
	if (!bIsDead){return;}
	UE_LOG(LogTemp, Warning, TEXT("客户端收到死亡 bIsDead=%s"), bIsDead?TEXT("true"):TEXT("false"));
	OnDeath.Broadcast();
}

void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor,
	float Damage,
	const UDamageType* DamageType,
	AController* InstigatedBy,
	AActor* DamageCasuer)
{

	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority()){return;}


	if(Damage <= 0.f || bIsDead){return;}

	CurrentHealth = FMath::Clamp((CurrentHealth- Damage), 0.f, MaxHealth);
	UE_LOG(LogTemp, Display, TEXT("%s Current HP:%f / %f"), *GetNameSafe(DamagedActor), CurrentHealth,MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth,MaxHealth);

	if (CurrentHealth <= 0.f)
	{
		bIsDead = true;
		OnDeath.Broadcast();

		UE_LOG(LogTemp, Display, TEXT("%s is Dead"), *GetNameSafe(DamagedActor));
	}

}

void UHealthComponent::OnRep_CurrentHealth()
{
	UE_LOG(LogTemp,Warning,TEXT("客户端收到生命值：%s HP = %f / %f"),*GetNameSafe(GetOwner()),CurrentHealth,MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

