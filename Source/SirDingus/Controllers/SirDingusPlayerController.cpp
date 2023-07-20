// Fill out your copyright notice in the Description page of Project Settings.


#include "SirDingusPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Kismet/KismetSystemLibrary.h"

#include "SirDingus/Characters/SirDingusCharacter.h"
#include "SirDingus/HUD/SirDingusHUD.h"
#include "SirDingus/HUD/CharacterOverlay.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void ASirDingusPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Get Reference to Hud
	PlayerHUD = Cast<ASirDingusHUD>(GetHUD());
}

void ASirDingusPlayerController::SetHUDHealth_Implementation(float Health, float MaxHealth)
{
	// if HUD is nullptr cast it, otherwise set to current self / do nothing
	PlayerHUD = PlayerHUD == nullptr ? Cast<ASirDingusHUD>(GetHUD()) : PlayerHUD;

	// Must Check: HUD, Overlay, Health bar, Health text
	bool bHUDValid = PlayerHUD &&
		PlayerHUD->CharacterOverlay &&
		PlayerHUD->CharacterOverlay->HealthBar &&
		PlayerHUD->CharacterOverlay->HealthText;
	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		PlayerHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		PlayerHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				5.f,
				FColor::Red,
				TEXT("SetHUDHealth Error - HUD is not Valid")
			);
		}
	}
}



//////////////////////////////////////////////////////////////////////////
/// Input

void ASirDingusPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent)) {

		/// Dodging - Incomplete
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &ASirDingusPlayerController::Dodge);
		//EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Completed, this, &ASirDingusCharacter::StopDodging);

		/// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASirDingusPlayerController::Move);

		/// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASirDingusPlayerController::Look);

		/// Attacking
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASirDingusPlayerController::Attack);

		/// Restart Game
		EnhancedInputComponent->BindAction(RestartGameAction, ETriggerEvent::Triggered, this, &ASirDingusPlayerController::RestartGame);

		/// Quit Game
		EnhancedInputComponent->BindAction(QuitGameAction, ETriggerEvent::Triggered, this, &ASirDingusPlayerController::QuitGame);

		/// Test Something
		EnhancedInputComponent->BindAction(TestAction, ETriggerEvent::Triggered, this, &ASirDingusPlayerController::TestSomething);
	}
}

void ASirDingusPlayerController::Dodge(const FInputActionValue& Value)
{
	/// Function Enter Message
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.f,
			FColor::Green,
			TEXT("ASirDingusCharacter::Dodge()")
		);
	}

	// Get character
	if (ASirDingusCharacter* character = Cast<ASirDingusCharacter>(GetPawn()))
	{
		// Initiate dodge
		character->Dodge();
	}
}

void ASirDingusPlayerController::Move(const FInputActionValue& Value)
{
	/// Function Enter Message
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(
	//		-1,
	//		1.f,
	//		FColor::Green,
	//		TEXT("ASirDingusPlayerController::Move()")
	//	);
	//}
	
	APawn* pawn = GetPawn();
	
	if (pawn)
	{
		/// Debug Message
		//if (GEngine)
		//{
		//	GEngine->AddOnScreenDebugMessage(
		//		-1,
		//		1.f,
		//		FColor::Green,
		//		TEXT("Pawn valid")
		//	);
		//}

		// input is a Vector2D
		FVector2D MovementVector = Value.Get<FVector2D>();

		// find out which way is forward
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		pawn->AddMovementInput(ForwardDirection, MovementVector.Y);
		pawn->AddMovementInput(RightDirection, MovementVector.X);

		/// DEBUG MESSAGE
		//if (GEngine)
		//{
		//	GEngine->AddOnScreenDebugMessage(
		//		-1,
		//		15.f,
		//		FColor::Yellow,
		//		FString::Printf(TEXT("Moving"))
		//	);
		//}
	}
	else
	{
		/// Debug Message
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				1.f,
				FColor::Red,
				TEXT("Pawn invalid")
			);
		}
	}
}

// where camera is looking NOT model
void ASirDingusPlayerController::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// add yaw and pitch input to controller
	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

void ASirDingusPlayerController::Attack(const FInputActionValue& Value)
{
	/// Function Enter Message
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			1.f,
			FColor::Green,
			TEXT("ASirDingusPlayerController::Attack()")
		);
	}

	// Get character
	if (ASirDingusCharacter* character = Cast<ASirDingusCharacter>(GetPawn()))
	{
		// Initiate attack
		character->SingleSwing();
	}
}

void ASirDingusPlayerController::RestartGame(const FInputActionValue& Value)
{
	/// Function Enter Message
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.f,
			FColor::Green,
			TEXT("ASirDingusCharacter::RestartGame()")
		);
	}
}

void ASirDingusPlayerController::QuitGame(const FInputActionValue& Value)
{
	/// Function Enter Message
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.f,
			FColor::Green,
			TEXT("ASirDingusCharacter::QuitGame()")
		);
	}

	//UKismetSystemLibrary::QuitGame
	//(
	//	this,
	//	this,
	//	EQuitPreference::Quit,
	//	true
	//);
}

void ASirDingusPlayerController::TestSomething(const FInputActionValue& Value)
{
	/// Function Enter Message
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.f,
			FColor::Green,
			TEXT("ASirDingusCharacter::TestSomething()")
		);
	}
}
