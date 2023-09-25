// Fill out your copyright notice in the Description page of Project Settings.


#include "SirDingusAnimInstance.h"
#include "SirDingusCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void USirDingusAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// Get character ref
	SirDingusCharacter = Cast<ASirDingusCharacter>(TryGetPawnOwner());

	// Get movement component ref
	if (SirDingusCharacter)
	{
		SirDingusCharacterMovement = SirDingusCharacter->GetCharacterMovement();
	}
}

void USirDingusAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (SirDingusCharacterMovement)
	{
		// Get Variables
		Speed = UKismetMathLibrary::VSizeXY(SirDingusCharacterMovement->Velocity);
		CharacterState = SirDingusCharacter->GetCharacterState();
	}
}
