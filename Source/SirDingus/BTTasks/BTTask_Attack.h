// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Attack.generated.h"

/**
 * 
 */
UCLASS()
class SIRDINGUS_API UBTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()

	UBTTask_Attack();

	/** Debug Booleans **/
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Debug|UBTTask_Attack", meta = (AllowPrivateAccess = "true"))
		bool bDebugMessages;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Debug|UBTTask_Attack", meta = (AllowPrivateAccess = "true"))
		bool bDebugLogs;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
