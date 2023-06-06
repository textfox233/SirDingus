// Copyright Epic Games, Inc. All Rights Reserved.

#include "SirDingusGameMode.h"
#include "SirDingusCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASirDingusGameMode::ASirDingusGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Content/Blueprints/Characters/BP_DungeonCharacter.uasset"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
