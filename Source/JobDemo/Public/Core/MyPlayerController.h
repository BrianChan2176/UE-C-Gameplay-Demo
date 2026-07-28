// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "MyPlayerController.generated.h"
class UInputMappingContext;
class UInputAction;
class UUserWidget;
class UPlayerHealthWidget;
UCLASS()
class JOBDEMO_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void HideCrosshair();

	UFUNCTION()
	void UpdateHealthBar(float CurrentHealth, float MaxHealth);
protected:
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	TObjectPtr<UInputMappingContext>MappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	TObjectPtr<UInputAction>IA_Move;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	TObjectPtr<UInputAction>IA_LookAround;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	TObjectPtr<UInputAction>IA_Jump;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	TObjectPtr<UInputAction>IA_Sprint;


	UPROPERTY(EditDefaultsOnly, Category = "Interact")
	TObjectPtr<UInputAction>IA_Interact;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UInputAction>IA_Shoot;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat")
	TSubclassOf<UUserWidget>CrosshairWidgetClass;
	UPROPERTY()
	TObjectPtr<UUserWidget>CrosshairWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<UPlayerHealthWidget>HealthWidgetClass;
	UPROPERTY()
	TObjectPtr<UPlayerHealthWidget>HealthWidget;
	
protected:
	virtual void BeginPlay()override;

	virtual void SetupInputComponent()override;

	UFUNCTION(BlueprintCallable, Category = "Interact")
	void TryInteract();


private:
	void Move(const FInputActionValue& Value);
	void LookAround(const FInputActionValue& Value);
	void StarJumping(const FInputActionValue& Value);
	void StopJumping(const FInputActionValue& Value);
	void StartSprinting(const FInputActionValue& Value);
	void StopSprinting(const FInputActionValue& Value);
	void Shoot(const FInputActionValue& Value);
};
