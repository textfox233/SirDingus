// Fill out your copyright notice in the Description page of Project Settings.


#include "SkeletonCharacter.h"
#include "SirDingus/Controllers/SirDingusAIController.h"

void ASkeletonCharacter::CharacterDeath()
{
	Super::CharacterDeath();

	/// Debug Msg
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(
	//		-1,
	//		2.f,
	//		FColor::Yellow,
	//		FString(TEXT("ASkeletonCharacter::HasDied()"))
	//	);
	//}

	// check for AI controller - not sure this is necessary but for now I'm paranoid so I'm doing it
	AController* rawController = Controller;

	if (rawController)
	{
		if (ASirDingusAIController * aiController = Cast<ASirDingusAIController>(rawController))
		{
			// update blackboard value
			aiController->SetIsAlive(false);
		}
	}
}
