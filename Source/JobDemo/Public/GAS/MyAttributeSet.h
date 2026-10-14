// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "MyAttributeSet.generated.h"

struct FGameplayEffectModCallbackData;
 
UCLASS()
class JOBDEMO_API UMyAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UMyAttributeSet();

    // 为 Mana 生成属性访问函数
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UMyAttributeSet, Mana)
    GAMEPLAYATTRIBUTE_VALUE_GETTER(Mana)
    GAMEPLAYATTRIBUTE_VALUE_SETTER(Mana)
    GAMEPLAYATTRIBUTE_VALUE_INITTER(Mana)

    // 为 MaxMana 生成属性访问函数
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UMyAttributeSet, MaxMana)
    GAMEPLAYATTRIBUTE_VALUE_GETTER(MaxMana)
    GAMEPLAYATTRIBUTE_VALUE_SETTER(MaxMana)
    GAMEPLAYATTRIBUTE_VALUE_INITTER(MaxMana)

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

protected:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana,Category = "Attributes")
	FGameplayAttributeData Mana;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Attributes")
	FGameplayAttributeData MaxMana;

    UFUNCTION()
    void OnRep_Mana(const FGameplayAttributeData& OldMana);
    UFUNCTION()
    void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);
};
