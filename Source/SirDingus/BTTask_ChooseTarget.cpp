// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChooseTarget.h"
#include "AIController.h"

EBTNodeResult::Type UBTTask_ChooseTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	APawn* TargetPawn;
	
	if (GetWorld()->GetNumPlayerControllers() > 1)
	{
		TargetPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		OwnerComp.GetAIOwner()->SetFocus(TargetPawn);
	}
	else
	// simple, pick player 1
	{
		TargetPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		OwnerComp.GetAIOwner()->SetFocus(TargetPawn);
	}

	UE_LOG(LogTemp, Warning, TEXT("set %s focus to %s"), *OwnerComp.GetAIOwner()->GetName(), *TargetPawn->GetName());
	UE_LOG(LogTemp, Warning, TEXT("Number of player controllers is %d"), GetWorld()->GetNumPlayerControllers());

	return EBTNodeResult::Succeeded;
}
