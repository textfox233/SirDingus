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

	// Gamemode Pointer
	class ASirDingusGameMode* CurrentGameMode;

	/** Camera Stuff **/
	// -- Camera Boom (positions the camera behind the character)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// -- Follow Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	

	/** MappingContext **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;


	/** Input Actions **/
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


	/** Equipped Weapon **/
	// -- Weapon Class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Loadout, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AWeapon> EquippedWeaponClass;

	// -- Actual Weapon
	UPROPERTY(BlueprintReadOnly, Category = Loadout, meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;


	//// Health
	//UPROPERTY(EditAnywhere, Category = Health, meta = (AllowPrivateAccess = "true"))
	//int MaxHealth = 100;
	//UPROPERTY(BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	//int Health;

public:
	ASirDingusCharacter();

	/** Character Death **/
	virtual void CharacterDeath();


	//// Blueprint Event for Attacking - Deprecated
	//UFUNCTION(BlueprintImplementableEvent, Category = "Attacks")
	//void AttackEvent();

	// When Character Takes Damage
	// - tried at first to bind a new function to OnTakeAnyDamage but ran into problems, decided instead to override AActor::TakeDamage()
	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	//UFUNCTION()
	//void DamageTaken(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	// -- Play Animations
	UFUNCTION(Server, Reliable, BlueprintCallable)
		void PlayAnimMontageServer(UAnimMontage* AnimMontage);
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
		void PlayAnimMontageMulticast(UAnimMontage* AnimMontage);

	// -- Animation Montages
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess))
		class UAnimMontage* BasicAttackMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess))
		class UAnimMontage* FlinchMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess))
		class UAnimMontage* DeathMontage;


	// -- Attacking (currently public so that AI can find it, may want to change this)
	void Attack(const FInputActionValue& Value);

protected:

	/** Input Functions **/
	// -- Dodging
	void Dodge(const FInputActionValue& Value);
	void StopDodging();

	// -- Movement
	void Move(const FInputActionValue& Value);

	// -- Looking
	void Look(const FInputActionValue& Value);
			
	//// -- Attacking
	//void Attack(const FInputActionValue& Value);


	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();


	// -- Is Character Alive
	UPROPERTY()
	bool bAlive = true;


	/** Trigger Melee Timer Refactored **/

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

	// -- Process Melee Hits
	UFUNCTION(BlueprintCallable)
	void ProcessMeleeHit(AActor* hitActor, bool bDebugLog = false);

	// -- Perform Weapon Arc via Line Traces
	UFUNCTION(BlueprintCallable)
	AActor* DrawWeaponArc(bool bDrawDebug = false, bool bDebugLog = false);

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

