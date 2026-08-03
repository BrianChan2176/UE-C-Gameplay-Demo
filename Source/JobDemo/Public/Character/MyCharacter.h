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

	void MoveByInput(const FRotator& ControlRotation, const FVector2D& MoveVector);

	UFUNCTION(BlueprintCallable, Category = "DoorKey")
	void TookDoorKey();
	UFUNCTION(BlueprintCallable, Category = "DoorKey")
	bool CheckHasDoorKey()const;

	//改成网络请求
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ShootDamage();//客户端本地射击意图


protected:
	// 改成网络请求
	UFUNCTION(Server, Reliable)
	void ServerShoot(FVector ClientTraceStart,FVector ClientDirenction);//客户端发送请求，服务器执行 请求_Implementation 判断合法性

	void PerformShoot(const FVector& ClientTraceStart, const FVector& ClientDirenction);//服务器权威射击

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DoorKey")
	bool bHasDoorKey = false;

	FTimerHandle CheckInteractTimer;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Component")
	TObjectPtr<UHealthComponent>HealthComponent;

	UFUNCTION()
	void HandleDeath();

	UFUNCTION()
	void HandleHealthChange(float CurrentHealth,float MaxHealth);
protected:

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
