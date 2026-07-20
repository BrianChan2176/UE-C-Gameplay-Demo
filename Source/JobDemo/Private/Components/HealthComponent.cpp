// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HealthComponent.h"
#include "GameFramework/Actor.h"
// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;

	if (AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);

	}
	
}


void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor,
	float Damage,
	const UDamageType* DamageType,
	AController* InstigatedBy,
	AActor* DamageCasuer)
{
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

