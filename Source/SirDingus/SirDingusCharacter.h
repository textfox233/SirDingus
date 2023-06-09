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
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MyAnimationInfo, meta = (AllowPrivateAccess = "true"))
	//bool bIsAttacking;

	// -- Restart Game
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RestartGameAction;
	
	// -- Quit Game
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* QuitGameAction;

	// -- Extra Test Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* TestAction;


	/** Equipped Weapon **/
	// -- Weapon Class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Loadout, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AWeapon> EquippedWeaponClass;

	// -- Actual Weapon
	UPROPERTY(BlueprintReadOnly, Category = Loadout, meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;

public:
	ASirDingusCharacter();

	/** Character Death **/
	virtual void CharacterDeath();


	/** Replication **/
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	/** Blueprint Event for Attacking - Deprecated **/
	//UFUNCTION(BlueprintImplementableEvent, Category = "Attacks")
	//void AttackEvent();


	/** Taking Damage **/
	// - tried at first to bind a new function to OnTakeAnyDamage but ran into problems, decided instead to override AActor::TakeDamage()
	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


	//** Playing Animations **/
	UFUNCTION(Server, Reliable, BlueprintCallable)
		void PlayAnimMontageServer(UAnimMontage* AnimMontage);
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
		void PlayAnimMontageMulticast(UAnimMontage* AnimMontage);


	/** Animation Montages **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess))
		class UAnimMontage* BasicAttackMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess))
		class UAnimMontage* FlinchMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess))
		class UAnimMontage* DeathMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess))
		class UAnimMontage* DodgeMontage;


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

	// -- Restart Game
	UFUNCTION(Server, Reliable)
	void RestartGame(const FInputActionValue& Value);
	
	// -- Quit Game
	void QuitGame(const FInputActionValue& Value);

	// -- Extra Test Action
	//UFUNCTION(BlueprintImplementableEvent, meta = (AllowPrivateAccess))
	void TestSomething(const FInputActionValue& Value);


	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	
	/** Debugging Functions **/

	// -- Return NetRole as an FString
	FString GetNetRole();

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


	/** Melee Functions **/

	// -- Process Melee Hits
	UFUNCTION(BlueprintCallable)
	bool ProcessMeleeHit(AActor* hitActor, bool bDebugLog = false);

	// -- Perform Weapon Arc via Line Traces
	UFUNCTION(BlueprintCallable)
	AActor* DrawWeaponArc(bool bDrawDebug = false, bool bDebugLog = false);


private:
	/** Is Character Alive **/
	UPROPERTY(Replicated)
	bool bAlive = true;


public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE void SetAlive(bool isAlive) { bAlive = isAlive; }
};

