// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_FaceTarget.h"
//#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

EBTNodeResult::Type UBTTaskNode_FaceTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (APawn* TargetPawn = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("TargetPlayer"))))
	{
		OwnerComp.GetAIOwner()->SetFocus(TargetPawn);
		return EBTNodeResult::Succeeded;
	}

	UE_LOG(LogTemp, Warning, TEXT("UBTTaskNode_FaceTarget::ExecuteTask | TargetPawn is nullptr"))
	return EBTNodeResult::Succeeded;
}
