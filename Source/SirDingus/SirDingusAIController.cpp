// Fill out your copyright notice in the Description page of Project Settings.


#include "SirDingusAIController.h"

#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

void ASirDingusAIController::BeginPlay()
{
	Super::BeginPlay();
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (AIBehavior)
	{
		RunBehaviorTree(AIBehavior);
		
		// initialise blackboard values				// key					// value
		GetBlackboardComponent()->SetValueAsVector	(TEXT("StartLocation"),	GetPawn()->GetActorLocation());
		GetBlackboardComponent()->SetValueAsBool	(TEXT("IsAlive"),		true);
	}

	// Debug Msg
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

void ASirDingusAIController::SetIsAlive(bool bValue)
{
	// Debug Msg
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Yellow,
			FString::Printf(TEXT("ASirDingusAIController::SetIsAlive(%s)"), (bValue ? TEXT("true") : TEXT("false")))
		);
	}

	// clear focus to stop rotation
	ClearFocus(EAIFocusPriority::Gameplay);
	// inform the blackboard of the AI's death
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsAlive"), bValue);
}

void ASirDingusAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	//	Can See Player?
	if(LineOfSightTo(PlayerPawn))
	{
		// Observe Player Position
		GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
		GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), PlayerPawn->GetActorLocation());

	}
	else
	{
		// cannot see player
		GetBlackboardComponent()->ClearValue(TEXT("PlayerLocation"));
	}
}

