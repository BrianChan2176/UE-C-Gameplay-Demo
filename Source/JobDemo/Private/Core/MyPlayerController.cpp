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
#include "UI/VictoryWidget.h"
#include "Engine/World.h"
#include "UI/AmmoWidget.h"
#include "Weapons/WeaponComponent.h"
#include "Weapons/WeaponBase.h"
#include "Weapons/WeaponDataAsset.h"
#include "GameFramework/Pawn.h"
#include "Core/MyGameInstance.h"
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

FText AMyPlayerController::GetAmmoText() const
{
	const FText EmptyText = FText::FromString(TEXT("-- / --"));

	// 找到这个控制器正在控制的角色
	const APawn* ControlledPawn = GetPawn();
	if (!IsValid(ControlledPawn)) { return EmptyText; }

	// 找到角色的武器组件
	const UWeaponComponent* WeaponComp =
		ControlledPawn->FindComponentByClass<UWeaponComponent>();

	if (!IsValid(WeaponComp)) { return EmptyText; }

	// 找到当前装备的武器
	const AWeaponBase* Weapon = WeaponComp->GetWeapon();
	if (!IsValid(Weapon))
	{
		return FText::FromString(TEXT("未装备"));
	}

	// 取得这把枪的配置
	const UWeaponDataAsset* Data = Weapon->GetWeaponData();
	if (!IsValid(Data)) { return EmptyText; }

	// 29 / 30
	return FText::FromString(FString::Printf(
		TEXT("%d / %d"),
		Weapon->GetCurrentAmmo(),
		Data->MagazineSize
	));
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (!IsLocalController()){return;}

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

	if (AmmoWidgetClass)
	{
		AmmoWidget = CreateWidget<UAmmoWidget>(this, AmmoWidgetClass);
		if (AmmoWidget)
		{
			AmmoWidget->AddToViewport();
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

void AMyPlayerController::ServerRestart_Implementation()
{
	UWorld* World = GetWorld();
	if (World) 
	{
		//死亡重置伤害倍率
		UMyGameInstance* GI =Cast<UMyGameInstance>(GetGameInstance());
		if (GI)
		{
			GI->ResetRun();
		}

		World->ServerTravel(TEXT("/Game/Maps/NewWorld"));
	}
}

void AMyPlayerController::ClientVictory_Implementation()
{
	if (!IsLocalController()) { return; }
	Victory();
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
	ControllCharacter->SetSprint(true);
}

void AMyPlayerController::StopSprinting(const FInputActionValue& Value)
{
	AMyCharacter* ControllCharacter = Cast<AMyCharacter>(GetPawn());
	if (!ControllCharacter) { return; }
	ControllCharacter->SetSprint(false);
}

void AMyPlayerController::Shoot(const FInputActionValue& Value)
{
	AMyCharacter* ControllCharacter = Cast<AMyCharacter>(GetPawn());
	if (!ControllCharacter) { return; }
	ControllCharacter->ShootDamage();
}

void AMyPlayerController::Victory()
{
	//ShowRestartWidget();//单机重开
	if (VictoryWidgetClass)
	{
		VictoryWidget = CreateWidget<UVictoryWidget>(this, VictoryWidgetClass);
		if (VictoryWidget)
		{
			VictoryWidget->AddToViewport(1000);
			HideCrosshair();
			SetIgnoreMoveInput(true);
			SetIgnoreLookInput(true);
			bShowMouseCursor = true;
		}
	}
}