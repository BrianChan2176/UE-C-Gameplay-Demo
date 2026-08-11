// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"
class USpringArmComponent;
class UCameraComponent;
class UUserWidget;
class UTextBlock;
class UHealthComponent;
UCLASS()
class JOBDEMO_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Interact")
	void TryInteract();
	UFUNCTION(Server,Reliable)
	void ServerInteract(AActor* Interactor);//客户端发送网络请求

	void PerformInteract(AActor* Interactor);//服务器权威交互


	void MoveByInput(const FRotator& ControlRotation, const FVector2D& MoveVector);


	//改成网络请求
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ShootDamage();//客户端本地射击意图


	void SetSprint(bool IsSprinting);//本体客户端奔跑意图
protected:
	// 改成网络请求
	UFUNCTION(Server, Reliable)
	void ServerShoot(FVector ClientTraceStart,FVector ClientDirection);//客户端发送请求，服务器执行 请求_Implementation 判断合法性

	void PerformShoot(const FVector& ClientTraceStart, const FVector& ClientDirection);//服务器权威射击

	UFUNCTION(NetMulticast,Unreliable)
	void MulticastPlayShootEffects(FVector TraceStart, FVector TraceEnd,bool bHitted);

	UFUNCTION(Server,Reliable)
	void ServerSprint(bool IsSprinting);//发送奔跑请求

	void ChangeMovementSpeed(bool IsSprinting);//服务器权威改变角色移动速度

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf <UUserWidget>InteractPromptWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget>InteractPromptWidget;

	UPROPERTY()
	TObjectPtr<AActor>CurrentInteractable;



	FTimerHandle CheckInteractTimer;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Component")
	TObjectPtr<UHealthComponent>HealthComponent;

	UFUNCTION()
	void HandleDeath();

	UFUNCTION()
	void HandleHealthChange(float CurrentHealth,float MaxHealth);

	void CheckInteract();

public:	

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float MovementSpeed = 400.f;

	float InteractRange = 600.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Combat")
	float ShootRange = 2000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	float Damage = 25.f;
};
