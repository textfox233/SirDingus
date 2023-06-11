// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChooseTarget.h"
#include "AIController.h"

EBTNodeResult::Type UBTTask_ChooseTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (GetWorld()->GetNumPlayerControllers() > 1)
	{
		APawn* TargetPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		OwnerComp.GetAIOwner()->SetFocus(TargetPawn);
	}
	else
	// simple, pick player 1
	{
		APawn* TargetPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		OwnerComp.GetAIOwner()->SetFocus(TargetPawn);
	}

	//UE_LOG(LogTemp, Warning, TEXT("set %s focus to %s"), *OwnerComp.GetAIOwner()->GetName() , *TargetPawn->GetName())

	return EBTNodeResult::Succeeded;
}
