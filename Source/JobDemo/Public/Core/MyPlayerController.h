// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "MyPlayerController.generated.h"
class UInputMappingContext;
class UInputAction;

UCLASS()
class JOBDEMO_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

protected:
	virtual void BeginPlay()override;
	virtual void SetupInputComponent()override;

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
private:
	void Move(const FInputActionValue& Value);
	void LookAround(const FInputActionValue& Value);
	void StarJumping(const FInputActionValue& Value);
	void StopJumping(const FInputActionValue& Value);
	void StartSprinting(const FInputActionValue& Value);
	void StopSprinting(const FInputActionValue& Value);

};
