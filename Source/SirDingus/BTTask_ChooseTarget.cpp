// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChooseTarget.h"
#include "GameFramework/GameState.h"
#include "AIController.h"
//#include <random>

EBTNodeResult::Type UBTTask_ChooseTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// upadte number of players
	_numPlayers = GetWorld()->GetNumPlayerControllers();

	if (_numPlayers)
	{
		APawn* TargetPawn;
	
		// more than 1 player
		if (_numPlayers > 1)
		{
			// Random Player
			//TargetPawn = GetRandomPlayerPawn();
			
			// Closest Player
			TargetPawn = GetClosestPlayerPawn(OwnerComp.GetAIOwner());

			if (TargetPawn)
			{
				// focus on them
				OwnerComp.GetAIOwner()->SetFocus(TargetPawn);
				return EBTNodeResult::Succeeded;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("TargetPawn is nullptr"));
			}
		}

		// only 1 player, pick that one
		else
		{
			TargetPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
			OwnerComp.GetAIOwner()->SetFocus(TargetPawn);
			return EBTNodeResult::Succeeded;
		}

		//UE_LOG(LogTemp, Warning, TEXT("set %s focus to %s"), *OwnerComp.GetAIOwner()->GetName(), *TargetPawn->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UBTTask_ChooseTarget::ExecuteTask() | ERROR | GetWorld()->GetNumPlayerControllers() returns False"));
	}
	return EBTNodeResult::Failed;
}

APawn* UBTTask_ChooseTarget::GetPlayerPawnByIndex(int Index)
{
	FConstPlayerControllerIterator i = GetWorld()->GetPlayerControllerIterator();

	// loop until the iterator reaches the index - actual loop is empty except for some debug logs
	for (; i.GetIndex() < Index; i++)
	{
		// Debug Here
		//UE_LOG(LogTemp, Warning, TEXT("Index is [%d]"), i.GetIndex());
		//UE_LOG(LogTemp, Warning, TEXT("Player [%d]: %s"), i.GetIndex(), *i->Get()->GetName());
	}

	/// Debug Logs
	//UE_LOG(LogTemp, Warning, TEXT("targetIndex is [%d]"), Index);
	//UE_LOG(LogTemp, Warning, TEXT("Final Index is [%d]"), i.GetIndex());
	//UE_LOG(LogTemp, Warning, TEXT("Player [%d]: %s"), i.GetIndex(), *i->Get()->GetName());

	// return player pawn at the desired index
	return i->Get()->GetPawn();
}

APawn* UBTTask_ChooseTarget::GetClosestPlayerPawn(AAIController* Owner)
{
	// loop until the iterator reaches the index - actual loop is empty except for some debug logs

	if(APawn* OwnerPawn = Owner->GetPawn())
	{
		float ShortestDistance = NULL;
		APawn* ClosestPlayerPawn = nullptr;

		FVector PlayerLocation;
		APawn* PlayerPawn;

		// Get owner location
		FVector OwnerLocation = OwnerPawn->GetActorLocation();

		// for every player
		for (FConstPlayerControllerIterator i = GetWorld()->GetPlayerControllerIterator(); i.GetIndex() < _numPlayers; i++)
		{
			/// Debug Logs
			//UE_LOG(LogTemp, Warning, TEXT("Index is [%d]"), i.GetIndex());
			//UE_LOG(LogTemp, Warning, TEXT("Player [%d]: %s"), i.GetIndex(), *i->Get()->GetName());

			/// Get Pawn
			PlayerPawn = i->Get()->GetPawn();
			if(PlayerPawn)
			{
				// Get Location
				PlayerLocation = PlayerPawn->GetActorLocation();

				// Get Distance (between owner and player)
				float CurrentDistance = FVector::Dist(OwnerLocation, PlayerLocation);

				/// Compare Distance to Previously Recorded
				// if shortest is NULL, recent is first recorded distance
				// otherwise if recent is smaller, it is the new shortest
				if (ShortestDistance == NULL || CurrentDistance < ShortestDistance )
				{
					// save distance for comparison 
					ShortestDistance = CurrentDistance;
					
					// save player pawn for return value
					ClosestPlayerPawn = PlayerPawn;
				}
				// if recent is larger than shortest do nothing; move on
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UBTTask_ChooseTarget::GetClosestPlayerPawn() | PlayerPawn is nullptr"));
			}
		}
		return ClosestPlayerPawn;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UBTTask_ChooseTarget::GetClosestPlayerPawn() | OwnerPawn is nullptr"));
	}
	
	/// Debug Logs
	//UE_LOG(LogTemp, Warning, TEXT("targetIndex is [%d]"), Index);
	//UE_LOG(LogTemp, Warning, TEXT("Final Index is [%d]"), i.GetIndex());
	//UE_LOG(LogTemp, Warning, TEXT("Player [%d]: %s"), i.GetIndex(), *i->Get()->GetName());
	
	// return player pawn at the desired index
	//return i->Get()->GetPawn();
	return nullptr;
}

APawn* UBTTask_ChooseTarget::GetRandomPlayerPawn()
{
	// select a random player
	int randomPlayerIndex = GetRandomPlayerIndex();
	UE_LOG(LogTemp, Warning, TEXT("GetRandomPlayerIndex() returned %d with %d players"), randomPlayerIndex, _numPlayers);
	
	// return that random player
	return GetPlayerPawnByIndex(randomPlayerIndex);

	//return nullptr;
}

int UBTTask_ChooseTarget::GetRandomPlayerIndex()
{
	// select a random player
	// get a random number between 1 and the total number of players
	int range = GetWorld()->GetNumPlayerControllers();
	int num = rand() % range;

	///Debug logs
	//UE_LOG(LogTemp, Warning, TEXT("Random number between %d and %d is %d"), 1, numPlayers, num);
	//UE_LOG(LogTemp, Warning, TEXT("Number of player controllers is %d"), GetWorld()->GetNumPlayerControllers());

	return num;
}
