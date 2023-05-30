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
		
		GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
		//GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
		GetBlackboardComponent()->SetValueAsInt(TEXT("Key"), 1);
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

void ASirDingusAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	//	Can See Player?
	if(LineOfSightTo(PlayerPawn))
	{
		// Observe Player Position
		//GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerActor"), PlayerPawn);
		GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
		GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), PlayerPawn->GetActorLocation());

	}
	else
	{
		// cannot see player
		GetBlackboardComponent()->ClearValue(TEXT("PlayerLocation"));
	}
}

