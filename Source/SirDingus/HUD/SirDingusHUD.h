// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SirDingusHUD.generated.h"

/**
 * 
 */
UCLASS()
class SIRDINGUS_API ASirDingusHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void DrawHUD() override;

};
