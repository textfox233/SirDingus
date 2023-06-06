// Copyright Epic Games, Inc. All Rights Reserved.

#include "SirDingusGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "SkeletonCharacter.h"
#include "UObject/ConstructorHelpers.h"

void ASirDingusGameMode::BeginPlay()
{
	Super::BeginPlay();

	HandleGameStart();
}

void ASirDingusGameMode::HandleGameStart()
{
	// Get the number of enemies in the level
	UE_LOG(LogTemp, Warning, TEXT("%d enemies in the level"), DetermineEnemyCount());

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
