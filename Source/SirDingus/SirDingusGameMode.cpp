// Copyright Epic Games, Inc. All Rights Reserved.

#include "SirDingusGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "SirDingusCharacter.h"
#include "SkeletonCharacter.h"
#include "UObject/ConstructorHelpers.h"

//void ASirDingusGameMode::ActorDied(APawn* DeadPawn)
void ASirDingusGameMode::CharacterDied(AActor* DeadActor)
{
	// Debug Msg
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(
	//		-1,
	//		2.f,
	//		FColor::Yellow,
	//		FString(TEXT("ASirDingusGameMode::ActorDied()"))
	//	);
	//}

	// if the dead actor is an AI
	if (ASkeletonCharacter* Skeleton = Cast<ASkeletonCharacter>(DeadActor))
	{
		// Debug Msg
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				2.f,
				FColor::Red,
				FString(TEXT("Dead actor is an AI character"))
			);
		}

		// 1. Handle Character Death
		Skeleton->CharacterDeath();

		// 2. Update number of enemies in the level
		EnemyCount--;

		// 3. Trigger game over with a victory if there are no more enemies
		if (EnemyCount == 0) { GameOver(true); }
	}
	else
	{
		// Debug Msg
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				2.f,
				FColor::Red,
				FString(TEXT("Dead actor is not an AI character"))
			);
		}
	}

	//// if the dead actor is a player
	//if (ASirDingusCharacter* player = Cast<ASirDingusCharacter>(DeadActor))
	//{
	//	// Debug Msg
	//	if (GEngine)
	//	{
	//		GEngine->AddOnScreenDebugMessage(
	//			-1,
	//			2.f,
	//			FColor::Green,
	//			FString(TEXT("Dead pawn is a player character"))
	//		);
	//	}
	//}

}

void ASirDingusGameMode::BeginPlay()
{
	Super::BeginPlay();

	HandleGameStart();
}

void ASirDingusGameMode::GameOver(bool bPlayerVictory)
{
	if (bPlayerVictory)
	{
		// Debug Msg
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Green,
				FString(TEXT("Player Wins!"))
			);
		}
	}
	else
	{
		// Debug Msg
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString(TEXT("Player Loses!"))
			);
		}
	}
}

void ASirDingusGameMode::HandleGameStart()
{
	// Get the number of enemies in the level
	EnemyCount = DetermineEnemyCount();
	UE_LOG(LogTemp, Warning, TEXT("%d enemies in the level"), EnemyCount);

	// get player?
	// get player controller?

	GameStart();

	// disable player?
}

int32 ASirDingusGameMode::DetermineEnemyCount()
{
	// get all skeletons in the level
	TArray<AActor*> AllEnemies;
	UGameplayStatics::GetAllActorsOfClass(this, ASkeletonCharacter::StaticClass(), AllEnemies);

	return AllEnemies.Num();
}
