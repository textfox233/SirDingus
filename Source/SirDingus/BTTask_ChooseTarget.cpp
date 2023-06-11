// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChooseTarget.h"
#include "GameFramework/GameState.h"
#include "AIController.h"

EBTNodeResult::Type UBTTask_ChooseTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	APawn* TargetPawn;
	
	if (GetWorld()->GetNumPlayerControllers() > 1)
	// more than 1 player
	{
		TargetPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		OwnerComp.GetAIOwner()->SetFocus(TargetPawn);

		FConstPlayerControllerIterator i = GetWorld()->GetPlayerControllerIterator();

		UE_LOG(LogTemp, Warning, TEXT("Player [%d]: %s"), i.GetIndex(), *i->Get()->GetName());

		i++;

		UE_LOG(LogTemp, Warning, TEXT("Player [%d]: %s"), i.GetIndex(), *i->Get()->GetName());
	}
	else
	// only 1 player, pick that one
	{
		TargetPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		OwnerComp.GetAIOwner()->SetFocus(TargetPawn);
	}

	UE_LOG(LogTemp, Warning, TEXT("set %s focus to %s"), *OwnerComp.GetAIOwner()->GetName(), *TargetPawn->GetName());
	UE_LOG(LogTemp, Warning, TEXT("Number of player controllers is %d"), GetWorld()->GetNumPlayerControllers());

	//GetWorld()->GetGameState()->PlayerArray;

	//for (FConstPlayerControllerIterator i = GetWorld()->GetPlayerControllerIterator(); i; i++)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Player [%d]: %s"), i.GetIndex(), *i->Get()->GetName());
	//}
	


	return EBTNodeResult::Succeeded;
}
