// Fill out your copyright notice in the Description page of Project Settings.


#include "SirDingusPlayerController.h"
#include "SirDingus/HUD/SirDingusHUD.h"
#include "SirDingus/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void ASirDingusPlayerController::BeginPlay()
{
	Super::BeginPlay();

	PlayerHUD = Cast<ASirDingusHUD>(GetHUD());
}

void ASirDingusPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
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
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
			-1,
			5.f,
			FColor::Red,
			TEXT("SetHUDHealth Error")
			);
		}
}