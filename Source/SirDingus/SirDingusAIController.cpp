// Fill out your copyright notice in the Description page of Project Settings.


#include "SirDingusAIController.h"
#include "Kismet/GameplayStatics.h"

void ASirDingusAIController::BeginPlay()
{
	Super::BeginPlay();
	//APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	//SetFocus(PlayerPawn);

	if (AIBehavior)
	{
		RunBehaviorTree(AIBehavior);

	}
	else if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString(TEXT("Behavior Tree Invalid"))
		);
	}
}

void ASirDingusAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	//APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	////Alive?
	////Yes
	////	See Player (In Range)?
	//if(LineOfSightTo(PlayerPawn))
	////	Yes
	//{
	//	//	Run At Player
	//	MoveToActor(PlayerPawn, 5.f);
	//	//	Attack
	//}
	////	No
	//else
	//{
	//	StopMovement();
	//	//	Return To Original
	//}
	////	No
	////	Do Nothing

}

