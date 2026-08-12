// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Door.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "Character/MyCharacter.h"
#include "Core/MyGameStateBase.h"
#include "Net/UnrealNetwork.h"
// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	PivotPoint = CreateDefaultSubobject<USceneComponent>(TEXT("PivotPoint"));
	PivotPoint->SetupAttachment(Root);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(PivotPoint);

	bReplicates = true;
	PivotPoint->SetIsReplicated(true);
}

void ADoor::Interact_Implementation(AActor* Interactor)
{
	if (!HasAuthority()) { return; }
	//服务器判断合法性有没有钥匙
	AMyCharacter* Character = Cast<AMyCharacter>(Interactor);
	if (Character)
	{
		AMyGameStateBase* MyGameState = GetWorld()->GetGameState<AMyGameStateBase>();
		if (!MyGameState) { return; }
		UE_LOG(LogTemp, Display, TEXT("队伍是否有key：%d"), MyGameState->bTeamHasDoorKey);
		if (MyGameState->bTeamHasDoorKey == false)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, TEXT("你当前没有钥匙，打不开门"));
			return;
		}
	}
	//开门
	if (bIsOpen == false) 
	{
		PivotPoint->AddWorldRotation(FRotator(0.f, 90.f, 0.f));
		bIsOpen = true;
	}
	else 
	{
		PivotPoint->AddWorldRotation(FRotator(0.f, -90.f, 0.f));
		bIsOpen = false;
	}
	
}

FText ADoor::GetInteractText_Implementation()const 
{
	return FText::FromString(TEXT("按 F 开门"));
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADoor, bIsOpen);
	DOREPLIFETIME(ADoor, PivotPoint);
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



