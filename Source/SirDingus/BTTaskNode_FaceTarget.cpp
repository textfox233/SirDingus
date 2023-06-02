// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_FaceTarget.h"
#include "AIController.h"

EBTNodeResult::Type UBTTaskNode_FaceTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	//APawn* OwnerPawn = OwnerComp.GetAIOwner()->GetPawn();

	APawn* TargetPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

	//FVector Direction = TargetPawn->GetTargetLocation() - OwnerPawn->GetTargetLocation();
	//Direction.Z = 0;

	//FRotator Rot = FRotationMatrix::MakeFromX(Direction).Rotator();

	//OwnerPawn->SetActorRotation(Rot);
	OwnerComp.GetAIOwner()->SetFocus(TargetPawn/*, EAIFocusPriority::Default*/);


	// may take multiple evaluations before facing the target
	//return EBTNodeResult::InProgress;
	return EBTNodeResult::Succeeded;
}
