// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MyPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Interaction/Interactable.h"
#include "Blueprint/UserWidget.h"
#include "UI/PlayerHealthWidget.h"
#include "UI/RestartWidget.h"
void AMyPlayerController::HideCrosshair()
{
	if (CrosshairWidget) 
	{
		CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMyPlayerController::UpdateHealthBar(float CurrentHealth, float MaxHealth)
{
	if (HealthWidget)
	{
		HealthWidget->SetHealth(CurrentHealth, MaxHealth);
	}
}

void AMyPlayerController::ShowRestartWidget()
{
	if (RestartWidgetClass)
	{
		RestartWidget = CreateWidget<URestartWidget>(this, RestartWidgetClass);
		if (!RestartWidget) { return; }

		RestartWidget->AddToViewport();
		bShowMouseCursor = true;
		
	}
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(GetLocalPlayer());
	if (LocalPlayer)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (Subsystem)
		{
			if (MappingContext)
			{
				Subsystem->AddMappingContext(MappingContext, 0);
			}
		}
	}

	if (CrosshairWidgetClass) 
	{
		CrosshairWidget = CreateWidget<UUserWidget>(this, CrosshairWidgetClass);
		if (CrosshairWidget)
		{
			CrosshairWidget->AddToViewport();
		}
	}

	if (HealthWidgetClass) 
	{
		HealthWidget = CreateWidget<UPlayerHealthWidget>(this, HealthWidgetClass);
		if (HealthWidget)
		{
			HealthWidget->AddToViewport();
			HealthWidget->SetHealth(1.f, 1.f);
		}
	}


}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
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

	if (IA_Shoot)
	{
		EnhancedInputComponent->BindAction(IA_Shoot, ETriggerEvent::Started, this, &AMyPlayerController::Shoot);
	}
}

void AMyPlayerController::TryInteract()
{
	AMyCharacter* ControllCharacter = Cast<AMyCharacter>(GetPawn());
	if (!ControllCharacter) { return; }
	ControllCharacter->TryInteract();
}

void AMyPlayerController::Move(const FInputActionValue& Value)
{
	FVector2D MoveVector = Value.Get<FVector2D>();

	AMyCharacter* ControllCharacter = Cast<AMyCharacter>(GetPawn());
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
	AMyCharacter* ControllCharacter = Cast<AMyCharacter>(GetPawn());
	if (!ControllCharacter) { return; }
	ControllCharacter->Jump();

}

void AMyPlayerController::StopJumping(const FInputActionValue& Value)
{
	AMyCharacter* ControllCharacter = Cast<AMyCharacter>(GetPawn());
	if (!ControllCharacter) { return; }
	ControllCharacter->StopJumping();

}



void AMyPlayerController::StartSprinting(const FInputActionValue& Value)
{
	AMyCharacter* ControllCharacter = Cast<AMyCharacter>(GetPawn());
	if (!ControllCharacter) { return; }
	ControllCharacter->GetCharacterMovement()->MaxWalkSpeed = 1200.f;

}

void AMyPlayerController::StopSprinting(const FInputActionValue& Value)
{
	AMyCharacter* ControllCharacter = Cast<AMyCharacter>(GetPawn());
	if (!ControllCharacter) { return; }
	ControllCharacter->GetCharacterMovement()->MaxWalkSpeed = ControllCharacter->MovementSpeed;

}

void AMyPlayerController::Shoot(const FInputActionValue& Value)
{
	AMyCharacter* ControllCharacter = Cast<AMyCharacter>(GetPawn());
	if (!ControllCharacter) { return; }
	ControllCharacter->ShootDamage();
}
