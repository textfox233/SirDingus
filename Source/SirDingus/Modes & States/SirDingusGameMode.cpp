// Copyright Epic Games, Inc. All Rights Reserved.

#include "SirDingusGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "SirDingus/Characters/SirDingusCharacter.h"
#include "AIController.h"
//#include "SirDingus/Characters/SkeletonCharacter.h"
//#include "SirDingus/Characters/KnightCharacter.h"
#include "UObject/ConstructorHelpers.h"
//#include "MultiplayerSessionsSubsystem.h"

void ASirDingusGameMode::AddToDeathToll(AController* DeadGuy)
{
	if (bDebugLogs)
		UE_LOG(LogTemp, Display, TEXT("Successfully accessed ASirDingusGameMode::CharacterDied()"));
	if (bDebugMessages && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Yellow,
			FString(TEXT("ASirDingusGameMode::ActorDied()"))
		);
	}
	
	// if the dead actor is an AI
	if (AAIController* DeadAI = Cast<AAIController>(DeadGuy))
	{
		if (bDebugLogs)
			UE_LOG(LogTemp, Warning, TEXT("AI %s has died."), *DeadAI->GetName()); 
		if (bDebugMessages && GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				2.f,
				FColor::Red,
				FString(TEXT("Dead actor is an AI character"))
			);
		}
	
		// 1. Update number of enemies left in the level
		_enemyCount--;
	
		// 2. Trigger game over with a victory if there are no more enemies
		if (_enemyCount == 0) { GameOver(true); }
	}
	
	// if the dead actor is a player
	else if (APlayerController* DeadPlayer = Cast<APlayerController>(DeadGuy))
	{
		if (bDebugLogs)
			UE_LOG(LogTemp, Warning, TEXT("Player %s has died."), *DeadPlayer->GetName());
		if (bDebugMessages && GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				2.f,
				FColor::Red,
				FString(TEXT("Dead actor is a player character"))
			);
		}
	
		// 1. Update number of dead players in the level
		_deadPlayerCount++;
	
		if (bDebugLogs)
		{
			UE_LOG(LogTemp, Warning, TEXT("%d players in the game"), GetNumPlayers());
			UE_LOG(LogTemp, Warning, TEXT("%d players are dead"), _deadPlayerCount);
		}

		// 2. Trigger game over with a loss if there are no more players
		if (_deadPlayerCount == GetNumPlayers()) { GameOver(false); }
	}
	
	// if casts fail
	else if (bDebugLogs)
			UE_LOG(LogTemp, Error, TEXT("Dead guy is neither an AI or a player controlled character"));
}

bool ASirDingusGameMode::RequestRestart(bool bDebug)
{
	// Debug Msg
	if (bDebug && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.f,
			FColor::Yellow,
			TEXT("ASirDingusGameMode::RequestRestart()")
		);
	}

	// must be game over to restart
	if (!_bGameInProgress) // matchstate::inprogress??
	{
		// Debug Msg
		if (bDebug && GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				3.f,
				FColor::Green,
				TEXT("Restart Initiated")
			);
		}

		/// reset level
		GetWorld()->ServerTravel("?Restart", false);

		return true;
	}
	// Debug Msg
	if (bDebug && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.f,
			FColor::Red,
			TEXT("Restart Denied")
		);
	}

	return false;
}

void ASirDingusGameMode::BeginPlay()
{
	Super::BeginPlay();

	// game hasn't started yet
	_bGameInProgress = false;

	HandleGameStart();
}

//void ASirDingusGameMode::GameOver(bool bPlayerVictory)
//{
//	if (bPlayerVictory)
//	{
//		// Debug Msg
//		if (GEngine)
//		{
//			GEngine->AddOnScreenDebugMessage(
//				-1,
//				15.f,
//				FColor::Green,
//				FString(TEXT("Player Wins!"))
//			);
//		}
//	}
//	else
//	{
//		// Debug Msg
//		if (GEngine)
//		{
//			GEngine->AddOnScreenDebugMessage(
//				-1,
//				15.f,
//				FColor::Red,
//				FString(TEXT("Player Loses!"))
//			);
//		}
//	}
//}

void ASirDingusGameMode::HandleGameStart()
{
	// Get the number of enemies in the level
	_enemyCount = DetermineEnemyCount();
	UE_LOG(LogTemp, Warning, TEXT("%d enemies in the level"), _enemyCount);

	// mark game as in progress
	_bGameInProgress = true;

	GameStart();
}

int32 ASirDingusGameMode::DetermineEnemyCount()
{
	// get all skeletons in the level
	TArray<AActor*> AllEnemies;
	UGameplayStatics::GetAllActorsOfClass(this, AAIController::StaticClass(), AllEnemies);

	return AllEnemies.Num();
}
