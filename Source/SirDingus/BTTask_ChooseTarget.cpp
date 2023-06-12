// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChooseTarget.h"
#include "GameFramework/GameState.h"
#include "AIController.h"
//#include <random>

EBTNodeResult::Type UBTTask_ChooseTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (int numPlayers = GetWorld()->GetNumPlayerControllers())
	{
		APawn* TargetPawn;
	
		// more than 1 player
		if (numPlayers > 1)
		{
			// select a random player
			int randomPlayerIndex = GetRandomPlayerIndex();
			UE_LOG(LogTemp, Warning, TEXT("GetRandomPlayerIndex() returned %d with %d players"), randomPlayerIndex, numPlayers);

			// get that random player
			TargetPawn = GetPlayerPawnByIndex(randomPlayerIndex);
			if (TargetPawn)
			{
				// focus on them
				OwnerComp.GetAIOwner()->SetFocus(TargetPawn);
				return EBTNodeResult::Succeeded;
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

	//GetWorld()->GetGameState()->PlayerArray;

	//for (FConstPlayerControllerIterator i = GetWorld()->GetPlayerControllerIterator(); i; i++)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Player [%d]: %s"), i.GetIndex(), *i->Get()->GetName());
	//}
	


	return EBTNodeResult::Succeeded;
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
