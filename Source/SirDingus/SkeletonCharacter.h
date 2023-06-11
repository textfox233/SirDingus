// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SirDingusCharacter.h"
#include "SkeletonCharacter.generated.h"

/**
 * 
 */
UCLASS()
class SIRDINGUS_API ASkeletonCharacter : public ASirDingusCharacter
{
	GENERATED_BODY()

	/** Character Death **/
	// -- Check for character death
	virtual bool HasDied() override;


};
