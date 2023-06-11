// Copyright Epic Games, Inc. All Rights Reserved.

#include "SirDingusGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "SirDingusCharacter.h"
#include "SkeletonCharacter.h"
#include "UObject/ConstructorHelpers.h"

void ASirDingusGameMode::ActorDied(APawn* DeadPawn)
//void ASirDingusGameMode::ActorDied(AActor* deadActor)
{
	// Debug Msg
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Yellow,
			FString(TEXT("ASirDingusGameMode::ActorDied()"))
		);
	}

	// if the dead actor is a player
	if (ASirDingusCharacter* player = Cast<ASirDingusCharacter>(DeadPawn->GetController()))
	{
		// Debug Msg
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				2.f,
				FColor::Green,
				FString(TEXT("Dead pawn is a player character"))
			);
		}
	}
	// if the dead actor is an AI
		// Debug Msg
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Red,
			FString(TEXT("Dead pawn is an AI character"))
		);
	}
}

void ASirDingusGameMode::BeginPlay()
{
	Super::BeginPlay();

	HandleGameStart();
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
