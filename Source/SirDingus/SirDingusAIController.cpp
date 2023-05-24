// Fill out your copyright notice in the Description page of Project Settings.


#include "SirDingusAIController.h"
#include "Kismet/GameplayStatics.h"

void ASirDingusAIController::BeginPlay()
{
	Super::BeginPlay();
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	SetFocus(PlayerPawn);
}

void ASirDingusAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	MoveToActor(PlayerPawn, 5.f);
}

