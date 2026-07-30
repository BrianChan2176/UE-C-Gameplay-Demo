// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interaction/Interactable.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Components/HealthComponent.h"
#include "Core/MyPlayerController.h"
#include "Components/CapsuleComponent.h"
// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);

	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
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

	GetWorldTimerManager().SetTimer(CheckInteractTimer, this, &AMyCharacter::CheckInteract, 0.3f, true);

	if (HealthComponent)
	{
		HealthComponent->OnHealthChanged.AddDynamic(this,&AMyCharacter::HandleHealthChange);
		HealthComponent->OnDeath.AddDynamic(this,&AMyCharacter::HandleDeath);
	}
}

void AMyCharacter::HandleDeath()
{
	// 停止交互检测
	GetWorldTimerManager().ClearTimer(CheckInteractTimer);
	if (InteractPromptWidget)
	{
		InteractPromptWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	// 停止玩家移动
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	// 禁止移动和转动镜头
	AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->SetIgnoreLookInput(true);
		PlayerController->SetIgnoreMoveInput(true);
		// 失去准星
		PlayerController->HideCrosshair();
		//显示重新开始widget
		PlayerController->ShowRestartWidget();
	}
	// Capsule不再参与碰撞
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 使用TutorialTPP网格体进入布娃娃
	USkeletalMeshComponent* MeshComponent = GetMesh();
	if (MeshComponent && MeshComponent->GetPhysicsAsset())
	{
		MeshComponent->SetCollisionProfileName(TEXT("Ragdoll"));
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshComponent->SetSimulatePhysics(true);
		MeshComponent->WakeAllRigidBodies();
	}

	UE_LOG(LogTemp,Display,TEXT("%s Player died"),*GetName());

}

void AMyCharacter::HandleHealthChange(float CurrentHealth, float MaxHealth)
{
	AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->UpdateHealthBar(CurrentHealth, MaxHealth);
	}
}

void AMyCharacter::CheckInteract()
{
	FVector ViewLocation;
	FRotator ViewRotator;
	AController* OwnerController = GetController();
	if (!OwnerController) { return; }

	OwnerController->GetPlayerViewPoint(ViewLocation, ViewRotator);
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
		AActor* HittedActor = HitResult.GetActor();
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
	//CheckInteract();
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMyCharacter::MoveByInput(const FRotator& ControlRotation, const FVector2D& MoveVector)
{
	if (!HealthComponent || HealthComponent->IsDead())
	{
		return;
	}

	const FRotator YawRotation = FRotator(0.f, ControlRotation.Yaw, 0.f);
	const FVector ForwardDirection=FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MoveVector.Y);
	AddMovementInput(RightDirection, MoveVector.X);
}

void AMyCharacter::TookDoorKey()
{
	bHasDoorKey = true;
}

bool AMyCharacter::CheckHasDoorKey() const
{
	return bHasDoorKey;
}

void AMyCharacter::ShootDamage()
{
	if (!HealthComponent || HealthComponent->IsDead())
	{
		return;
	}

	AController* OwnerController=GetController();
	if (!OwnerController) { return; }

	FVector ViewLocation;
	FRotator ViewRotator;
	OwnerController->GetPlayerViewPoint(ViewLocation, ViewRotator);

	FVector Start = ViewLocation;

	FVector ForwardDirection = ViewRotator.Vector();
	FVector End = Start + ForwardDirection * ShootRange;

	FHitResult HitResult;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHitted = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
	#if !UE_BUILD_SHIPPING
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.f, 0, 2.f);
	#endif
	if (bHitted)
	{
		#if !UE_BUILD_SHIPPING
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 12, FColor::Green, false, 2.0f);
		#endif
		AActor* HittedActor = HitResult.GetActor();
		if (!HittedActor) { return; }
		UGameplayStatics::ApplyDamage(HittedActor, Damage, OwnerController, this, UDamageType::StaticClass());

	}

}

void AMyCharacter::TryInteract()
{
	if (!HealthComponent || HealthComponent->IsDead())
	{
		return;
	}

	AController* OwnerController = GetController();
	if (!OwnerController) { return; }

	FVector ViewLocation;
	FRotator ViewRotator;
	OwnerController->GetPlayerViewPoint(ViewLocation, ViewRotator);

	FVector Start = ViewLocation;

	FVector ForwardDirection = ViewRotator.Vector();
	FVector End = Start + ForwardDirection * InteractRange;

	FHitResult HitResult;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHitted = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
	#if !UE_BUILD_SHIPPING
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.f, 0, 2.f);
	#endif
	if (bHitted)
	{
		#if !UE_BUILD_SHIPPING
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 12, FColor::Green, false, 2.0f);
		#endif
		AActor* HittedActor = HitResult.GetActor();
		if (HittedActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
		{
			IInteractable::Execute_Interact(HittedActor, this);
		}
	}
}

