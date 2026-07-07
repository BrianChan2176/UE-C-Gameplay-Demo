// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MyPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Interaction/Interactable.h"
void AMyPlayerController::BeginPlay()
{
	TObjectPtr<ULocalPlayer>LocalPlayer = Cast<ULocalPlayer>(GetLocalPlayer());
	if (LocalPlayer)
	{
		TObjectPtr<UEnhancedInputLocalPlayerSubsystem>Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (Subsystem)
		{
			if (MappingContext)
			{
				Subsystem->AddMappingContext(MappingContext, 0);
			}
		}
	}
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	TObjectPtr<UEnhancedInputComponent>EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComponent) { return; }

	if (IA_Move) 
	{
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AMyPlayerController::Move);
	}

	if (IA_LookAround)
	{
		EnhancedInputComponent->BindAction(IA_LookAround, ETriggerEvent::Triggered, this, &AMyPlayerController::LookAround);
	}

	if (IA_Jump)
	{
		EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Started, this, &AMyPlayerController::StarJumping);
		EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Completed, this, &AMyPlayerController::StopJumping);
	}

	if (IA_Sprint)
	{
		EnhancedInputComponent->BindAction(IA_Sprint, ETriggerEvent::Started, this, &AMyPlayerController::StartSprinting);
		EnhancedInputComponent->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &AMyPlayerController::StopSprinting);
	}

	if (IA_Interact)
	{
		EnhancedInputComponent->BindAction(IA_Interact, ETriggerEvent::Completed, this, &AMyPlayerController::TryInteract);
	}
}

void AMyPlayerController::TryInteract()
{
	TObjectPtr<AMyCharacter>ControllCharacter = Cast<AMyCharacter>(GetPawn());
	if (!ControllCharacter) { return; }
	ControllCharacter->TryInteract();
}

void AMyPlayerController::Move(const FInputActionValue& Value)
{
	FVector2D MoveVector = Value.Get<FVector2D>();

	TObjectPtr<AMyCharacter>ControllCharacter = Cast<AMyCharacter>(GetPawn());
	if (!ControllCharacter) { return; }
	ControllCharacter->MoveByInput(GetControlRotation(), MoveVector);

}

void AMyPlayerController::LookAround(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();
	AddYawInput(LookVector.X);
	AddPitchInput(LookVector.Y * -1);
}

void AMyPlayerController::StarJumping(const FInputActionValue& Value)
{
	TObjectPtr<AMyCharacter>ControllCharacter = Cast<AMyCharacter>(GetPawn());
	if (!ControllCharacter) { return; }
	ControllCharacter->Jump();

}

void AMyPlayerController::StopJumping(const FInputActionValue& Value)
{
	TObjectPtr<AMyCharacter>ControllCharacter = Cast<AMyCharacter>(GetPawn());
	if (!ControllCharacter) { return; }
	ControllCharacter->StopJumping();

}



void AMyPlayerController::StartSprinting(const FInputActionValue& Value)
{
	TObjectPtr<AMyCharacter>ControllCharacter = Cast<AMyCharacter>(GetPawn());
	if (!ControllCharacter) { return; }
	ControllCharacter->GetCharacterMovement()->MaxWalkSpeed = 1200.f;

}

void AMyPlayerController::StopSprinting(const FInputActionValue& Value)
{
	TObjectPtr<AMyCharacter>ControllCharacter = Cast<AMyCharacter>(GetPawn());
	if (!ControllCharacter) { return; }
	ControllCharacter->GetCharacterMovement()->MaxWalkSpeed = ControllCharacter->MovementSpeed;

}