// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_FaceTarget.h"
#include "AIController.h"

EBTNodeResult::Type UBTTaskNode_FaceTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* TargetPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	OwnerComp.GetAIOwner()->SetFocus(TargetPawn);

	//UE_LOG(LogTemp, Warning, TEXT("set %s focus to %s"), *OwnerComp.GetAIOwner()->GetName() , *TargetPawn->GetName())

	return EBTNodeResult::Succeeded;
}
