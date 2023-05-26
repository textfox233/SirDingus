// Fill out your copyright notice in the Description page of Project Settings.


#include "SirDingusAIController.h"

#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

void ASirDingusAIController::BeginPlay()
{
	Super::BeginPlay();
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	//SetFocus(PlayerPawn);

	if (AIBehavior)
	{
		RunBehaviorTree(AIBehavior);
		
		//APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

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

	////Alive?
	////Yes
	////	See Player (In Range)?
	if(LineOfSightTo(PlayerPawn))
		//Yes
	{
			//Run At Player
		//MoveToActor(PlayerPawn, 5.f);
			//Attack

		 //Observe Player Position
		//GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
		GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerActor"), PlayerPawn);
		GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), PlayerPawn->GetActorLocation());

	}
	////	No
	//else
	//{
	//	//StopMovement();
	//	//	Return To Original

	//	// cannot see player
	//	//GetBlackboardComponent()->ClearValue(TEXT("PlayerLocation"));
	//	//GetBlackboardComponent()->ClearValue(TEXT("PlayerActor"));
	//}
	////	No
	////	Do Nothing

}

