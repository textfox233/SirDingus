// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "AIController.h"
#include "SirDingus/Characters/SirDingusCharacter.h"

UBTTask_Attack::UBTTask_Attack()
{
	//NodeName = TEXT("Attack");
}


EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// Debug Msg
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(
	//		-1,
	//		15.f,
	//		FColor::Yellow,
	//		FString(TEXT("Enemy Attack Task"))
	//	);
	//}

	APawn* OwnerPawn = OwnerComp.GetAIOwner()->GetPawn();

	// Null Test
	if (OwnerPawn)
	{
		ASirDingusCharacter* OwnerCharacter = Cast<ASirDingusCharacter>(OwnerPawn);

		// Null Test
		if (OwnerCharacter)
		{
			// clear focus
			OwnerComp.GetAIOwner()->ClearFocus(EAIFocusPriority::Default);

			// initate attack
			OwnerCharacter->Attack();

			// return success
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}