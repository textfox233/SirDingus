// Fill out your copyright notice in the Description page of Project Settings.


#include "SirDingusPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SirDingus/Characters/SirDingusCharacter.h"
#include "SirDingus/HUD/SirDingusHUD.h"
#include "SirDingus/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

//////////////////////////////////////////////////////////////////////////
/// Input

void ASirDingusPlayerController::SetupInputComponent()
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent)) {

		/// Dodging - Incomplete
		//EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &ASirDingusCharacter::Dodge);
		//EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Completed, this, &ASirDingusCharacter::StopDodging);

		/// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASirDingusPlayerController::Move);
		//EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASirDingusCharacter::Move);

		/// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASirDingusPlayerController::Look);

		/// Attacking
		//EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASirDingusCharacter::Attack);

		/// Restart Game
		//EnhancedInputComponent->BindAction(RestartGameAction, ETriggerEvent::Triggered, this, &ASirDingusCharacter::RestartGame);

		/// Quit Game
		//EnhancedInputComponent->BindAction(QuitGameAction, ETriggerEvent::Triggered, this, &ASirDingusCharacter::QuitGame);

		/// Test Something
		//EnhancedInputComponent->BindAction(TestAction, ETriggerEvent::Triggered, this, &ASirDingusCharacter::TestSomething);
	}
}

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