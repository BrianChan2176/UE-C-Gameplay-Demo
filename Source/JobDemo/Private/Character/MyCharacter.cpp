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

	//支持网络复制
	bReplicates = true;
	SetReplicateMovement(true);
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
	if (!IsLocallyControlled()) { return; }
	if (!HealthComponent || HealthComponent->IsDead()){return;}

	AController* OwnerController = GetController();
	if (!OwnerController) { return; }

	FVector ClientViewLocation;
	FRotator ClientViewRotator;
	OwnerController->GetPlayerViewPoint(ClientViewLocation, ClientViewRotator);

	FVector ClientTraceStart = ClientViewLocation;

	FVector ClientForwardDirection = ClientViewRotator.Vector();
	UE_LOG(LogTemp, Display, TEXT("%s射击输入意图   有无Authority:%d   是否本地控制器:%d"), *GetName(), HasAuthority(), IsLocallyControlled());
	if (HasAuthority())//如果有服务器权威
	{
		PerformShoot(ClientTraceStart, ClientForwardDirection);//服务器权威射击
	}
	else 
	{
		ServerShoot(ClientTraceStart, ClientForwardDirection);//网络入口，客户端发送射击请求，服务器执行 请求_Implementation 判断合法性
	}

}

void AMyCharacter::ServerShoot_Implementation(FVector ClientTraceStart, FVector ClientDirection)//服务器先判断client射击合法性，之后才权威射击
{
	if (!HealthComponent || HealthComponent->IsDead()) { return; }
	if (ClientDirection.IsNearlyZero()) { return; }

	//先判断客户端传过来射线的开始是不是在服务器角色附近，防止客户端作弊射线在不合理的地方开始打出
	const float MaxTraceStartOffSet = 600.f;
	const FVector PawnCameraLocation=GetPawnViewLocation();
	float ClientServerTraceDistance=FVector::Distance(ClientTraceStart, PawnCameraLocation);
	if (ClientServerTraceDistance> MaxTraceStartOffSet)
	{
		UE_LOG(LogTemp, Display, TEXT("客户端射线开端不合理,服务器不打射线"));
		return;
	}

	//客户端的射线开始在服务器误差范围内，可以打射线
	UE_LOG(LogTemp, Display, TEXT("客户端射击合法，服务器权威射击"));
	const FVector SafeDirection = ClientDirection.GetSafeNormal();   // 保留方向，强制把长度变成1
	PerformShoot(ClientTraceStart, SafeDirection);
}

void AMyCharacter::PerformShoot(const FVector& ClientTraceStart, const FVector& ClientDirection)//服务器权威射击
{
	if (!HasAuthority()) { return; }//只能在服务器调用，限制客户端调用
	if (!HealthComponent || HealthComponent->IsDead()){return;}
	if (ClientDirection.IsNearlyZero()) { return; }

	AController* OwnerController=GetController();
	if (!OwnerController) {  return; }
	const FVector SafeDirection = ClientDirection.GetSafeNormal();   // 保留方向，强制把长度变成1
	FVector End = ClientTraceStart + SafeDirection * ShootRange;

	FHitResult HitResult;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHitted = GetWorld()->LineTraceSingleByChannel(HitResult, ClientTraceStart, End, ECC_Visibility, Params);
	#if !UE_BUILD_SHIPPING
	DrawDebugLine(GetWorld(), ClientTraceStart, End, FColor::Red, false, 2.f, 0, 2.f);
	#endif
	if (bHitted)
	{
		#if !UE_BUILD_SHIPPING
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 12, FColor::Green, false, 2.0f);
		#endif
		AActor* HittedActor = HitResult.GetActor();
		if (!HittedActor) { return; }
		UGameplayStatics::ApplyDamage(HittedActor, Damage, OwnerController, this, UDamageType::StaticClass());
		UE_LOG(LogTemp,Warning,TEXT("服务器对 %s 造成了 %f 伤害"),*HittedActor->GetName(), Damage);
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

