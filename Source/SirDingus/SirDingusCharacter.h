// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "SirDingusCharacter.generated.h"

UCLASS(config=Game)
class ASirDingusCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Input Actions */
	// -- Dodge
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DodgeAction;

	// -- Move
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	// -- Look
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	// -- Attack
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MyAnimationInfo, meta = (AllowPrivateAccess = "true"))
	bool bIsAttacking;

	/** Equipped Weapon Class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Loadout, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AWeapon> EquippedWeaponClass;

	/** Actual Equipped Weapon */
	UPROPERTY( BlueprintReadOnly, Category = Loadout, meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;

	// Health
	UPROPERTY(EditAnywhere, Category = Health, meta = (AllowPrivateAccess = "true"))
	int MaxHealth = 100;
	UPROPERTY(BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	int Health;

public:
	ASirDingusCharacter();

	UFUNCTION(BlueprintImplementableEvent, Category = "Attacks")
	void AttackEvent();

	UFUNCTION()
	void DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

protected:

	/** Called for dodging input */
	void Dodge(const FInputActionValue& Value);
	void StopDodging();
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			
	/** Called for attacking input */
	void Attack(const FInputActionValue& Value);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	// Apply damage & check for character death
	UFUNCTION(BlueprintCallable)
	virtual bool IsDead(int dmg);

	// Process Melee Hits
	UFUNCTION(BlueprintCallable)
		void ProcessMeleeHit(AActor* hitActor);

	// Perform Weapon Arc via Line Traces
	UFUNCTION(BlueprintCallable)
		AActor* DrawWeaponArc(bool bDrawDebug = false);

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

