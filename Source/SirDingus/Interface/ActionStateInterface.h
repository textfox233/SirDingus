// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SirDingus/Characters/CharacterTypes.h"
#include "ActionStateInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UActionStateInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SIRDINGUS_API IActionStateInterface
{
	GENERATED_BODY()
	
	EActionState ActionState = EActionState::EAS_Unoccupied;

public:
	UFUNCTION(BlueprintCallable)
	virtual void UpdateActionState(const EActionState State) { ActionState = State; };

	const EActionState GetActionState() { return ActionState; };

	UFUNCTION(BlueprintCallable)
	virtual void PrintActionState(const FString ObjectName)
	{
		if (GEngine)
		{
			FString msg = FString(ObjectName + TEXT("::ActionState = ") + 
				UEnum::GetValueAsString(ActionState));
			GEngine->AddOnScreenDebugMessage(
				1,
				2.f,
				FColor::Green,
				msg
			);
		}
	}

	bool CanMove() { return ActionState == EActionState::EAS_Unoccupied; }
};
