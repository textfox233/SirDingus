// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MeleeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIRDINGUS_API UMeleeComponent : public UActorComponent
{
	GENERATED_BODY()

	/** Equipped Weapon **/
	// -- Weapon Class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Loadout, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class AWeapon> EquippedWeaponClass;

	// -- Actual Weapon
	UPROPERTY(BlueprintReadOnly, Category = Loadout, meta = (AllowPrivateAccess = "true"))
		AWeapon* EquippedWeapon;

	// -- Debug booleans
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = Debug, meta = (AllowPrivateAccess = "true"))
		bool bDrawLineTrace;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = Debug, meta = (AllowPrivateAccess = "true"))
		bool bDebugMessages;
public:
	// Sets default values for this component's properties
	UMeleeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/** Melee Animation Timer (Refactored) **/

	// -- Timer Handle
	FTimerHandle MeleeTraceHandle;

	// -- Start
	UFUNCTION(BlueprintCallable)
		void MeleeTraceStart();

	// -- InProgress
	UFUNCTION()
		void MeleeTraceInProgress();

	// -- Stop
	UFUNCTION(BlueprintCallable)
		void MeleeTraceEnd();

	// -- Determine if anything was hit, and if damage should be applied. Returns TRUE if any damage was dealt
	bool ProcessMeleeHit(AActor* hitActor, bool bDebugLog = false);

	// -- Draw a line trace to track a weapon's movement and detect hit events
	AActor* DrawWeaponArc(bool bDrawDebug = false, bool bDebugLog = false);
};
