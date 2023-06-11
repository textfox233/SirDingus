// Fill out your copyright notice in the Description page of Project Settings.


#include "SkeletonCharacter.h"
#include "SirDingusAIController.h"

bool ASkeletonCharacter::HasDied()
{
	bool dead = Super::HasDied();

	//// Debug Msg
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(
	//		-1,
	//		2.f,
	//		FColor::Yellow,
	//		FString(TEXT("ASkeletonCharacter::HasDied()"))
	//	);
	//}

	if (dead)
	{
		// check for AI controller
		AController* rawController = Controller;
		ASirDingusAIController* aiController;

		if (rawController)
		{
			aiController = Cast<ASirDingusAIController>(rawController);

			if (aiController)
			{
				// update blackboard value
				aiController->SetIsAlive(false);
			}
		}
	}

	return dead;
}
