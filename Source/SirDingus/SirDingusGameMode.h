// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SirDingusGameMode.generated.h"

UCLASS(minimalapi)
class ASirDingusGameMode : public AGameModeBase
{
	GENERATED_BODY()

	// Default game mode
	//	Win: - Clear all enemies
	//	Loss: - all players dead

public:
	//void ActorDied(APawn* deadActor);
	void CharacterDied(class AActor* DeadActor);

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void GameStart();
	
	UFUNCTION(BlueprintImplementableEvent)
	void GameOver(bool bPlayerVictory);

private:
	void HandleGameStart();
	
	int32 DetermineEnemyCount();
	int32 _enemyCount = 0;

	int32 _deadPlayerCount = 0;
};



