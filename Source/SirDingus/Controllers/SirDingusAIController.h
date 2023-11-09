// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SirDingus/Interface/ActionStateInterface.h"
#include "AIController.h"
#include "SirDingusAIController.generated.h"

/**
 * 
 */
UCLASS()
class SIRDINGUS_API ASirDingusAIController : public AAIController, public IActionStateInterface
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;
	void SetIsAlive(bool value);

	APawn* ChooseTarget();

	virtual void UpdateActionState(const EActionState State) override;

private:
	UPROPERTY(EditAnywhere)
	class UBehaviorTree* AIBehavior;

	bool CanSeeAnyPlayer();
	void LostSightOfTarget();

	APawn* GetPlayerPawnByIndex(int Index);
	APawn* GetClosestPlayerPawn();
	APawn* GetRandomPlayerPawn();

	int GetRandomPlayerIndex();
	int _numPlayers = 1;

	/** Debug Area **/
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Debug|SirDingusAIController", meta = (AllowPrivateAccess = "true"))
	bool bDebugMessages = false;
};
