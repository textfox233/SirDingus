// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MeleeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIRDINGUS_API UMeleeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMeleeComponent();

	void PerformAttack();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/** Melee Animation Timer (Refactored) **/
	FTimerHandle MeleeTraceHandle;
	UFUNCTION(BlueprintCallable)
		void MeleeTraceStart();
	UFUNCTION()
		void MeleeTraceInProgress();
	UFUNCTION(BlueprintCallable)
		void MeleeTraceEnd();

	// -- Determine if anything was hit, and if damage should be applied. Returns TRUE if any damage was dealt
	bool ProcessMeleeHit(FHitResult* hitResult);

	// -- Draw a line trace to track a weapon's movement and detect hits
	void DrawWeaponArc();

private:
	/** Equipped Weapon **/
	// -- Weapon Class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Loadout, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AWeapon> EquippedWeaponClass;

	// -- Actual Weapon
	UPROPERTY(BlueprintReadOnly, Category = Loadout, meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;

	// -- Attack Animations
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess))
	class UAnimMontage* AttacksMontage;

	// -- Debug booleans
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Debug|MeleeComponent", meta = (AllowPrivateAccess = "true"))
	bool bDrawDebug;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Debug|MeleeComponent", meta = (AllowPrivateAccess = "true"))
	bool bDebugMessages;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Debug|MeleeComponent", meta = (AllowPrivateAccess = "true"))
	bool bDebugLog;
};
