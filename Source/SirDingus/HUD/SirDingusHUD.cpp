// Fill out your copyright notice in the Description page of Project Settings.


#include "SirDingusHUD.h"
#include "GameFramework/PlayerController.h"
#include "CharacterOverlay.h"

void ASirDingusHUD::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.f,
			FColor::Green,
			TEXT("HUD::BeginPlay")
		);
	}

	AddCharacterOverLay();
}

void ASirDingusHUD::AddCharacterOverLay()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.f,
			FColor::Green,
			TEXT("HUD::AddCharacterOverLay")
		);
	}

	APlayerController* playerController = GetOwningPlayerController();
	if (playerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(playerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				1,
				5.f,
				FColor::Red,
				TEXT("HUD Error")
			);
		}
	}
}

void ASirDingusHUD::DrawHUD()
{
	Super::DrawHUD();
}

