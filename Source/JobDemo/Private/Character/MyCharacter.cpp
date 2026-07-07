// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interaction/Interactable.h"
#include "Blueprint/UserWidget.h"
// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);

	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (InteractPromptWidgetClass)
	{
		InteractPromptWidget = CreateWidget<UUserWidget>(GetWorld(), InteractPromptWidgetClass);
		if (InteractPromptWidget)
		{
			InteractPromptWidget->AddToViewport(9999);
			InteractPromptWidget->SetVisibility(ESlateVisibility::Hidden);
			
		}
	}
}

void AMyCharacter::CheckInteract()
{
	FVector ViewLocation;
	FRotator ViewRotator;
	GetController()->GetPlayerViewPoint(ViewLocation, ViewRotator);

	FVector Start = ViewLocation;

	FVector ForwardDirection = ViewRotator.Vector();
	FVector End = Start + ForwardDirection * InteractRange;

	FHitResult HitResult;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHitted = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
	//DrawDebugLine(GetWorld(), Start, End, FColor::Black, false, 2.f, 0, 0.0f);//每帧打射线太多了先不显示了
	if (bHitted)
	{
		//DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 12, FColor::Green, false, 2.0f);//每帧画太多了先不显示了
		TObjectPtr<AActor>HittedActor = HitResult.GetActor();
		bool bInteractable = HittedActor && HittedActor->GetClass()->ImplementsInterface(UInteractable::StaticClass());

		if (bInteractable)
		{
			CurrentInteractable = HittedActor;
			if (InteractPromptWidget) 
			{
				InteractPromptWidget->SetVisibility(ESlateVisibility::Visible);
			
			}
		}
		else 
		{
			CurrentInteractable = nullptr;
			if (InteractPromptWidget)
			{
				InteractPromptWidget->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
	else
	{
		CurrentInteractable = nullptr;
		if (InteractPromptWidget)
		{
			InteractPromptWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}


// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckInteract();
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMyCharacter::MoveByInput(const FRotator& ControlRotation, const FVector2D& MoveVector)
{
	const FRotator YawRotation = FRotator(0.f, ControlRotation.Yaw, 0.f);
	const FVector ForwardDirection=FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MoveVector.Y);
	AddMovementInput(RightDirection, MoveVector.X);
}

void AMyCharacter::TryInteract()
{
	FVector ViewLocation;
	FRotator ViewRotator;
	GetController()->GetPlayerViewPoint(ViewLocation, ViewRotator);

	FVector Start = ViewLocation;

	FVector ForwardDirection = ViewRotator.Vector();
	FVector End = Start + ForwardDirection * InteractRange;

	FHitResult HitResult;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHitted = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.f, 0, 2.f);
	if (bHitted)
	{
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 12, FColor::Green, false, 2.0f);
		TObjectPtr<AActor>HittedActor = HitResult.GetActor();
		if (HittedActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
		{
			IInteractable::Execute_Interact(HittedActor, this);
		}
	}
}