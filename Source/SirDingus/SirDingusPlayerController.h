// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SirDingusPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SIRDINGUS_API ASirDingusPlayerController : public APlayerController
{
	GENERATED_BODY()

		ASirDingusPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI, meta = (AllowPrivateAccess = "true")) TSubclassOf<class UUserWidget> widgetHUD;
	UUserWidget* widgetHUDInstance;


};
