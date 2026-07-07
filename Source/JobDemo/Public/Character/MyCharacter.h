// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"
class USpringArmComponent;
class UCameraComponent;
class UUserWidget;
class UTextBlock;
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


protected:
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
protected:

	void CheckInteract();
public:	

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float MovementSpeed = 400.f;

	float InteractRange = 400.f;

};
