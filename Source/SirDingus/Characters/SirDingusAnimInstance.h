// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"
#include "SirDingusAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SIRDINGUS_API USirDingusAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation();
	virtual void NativeUpdateAnimation(float DeltaTime);

	UPROPERTY(BlueprintReadOnly)
	class ASirDingusCharacter* SirDingusCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* SirDingusCharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float Speed = 0.f;

	ECharacterState CharacterState;
};
